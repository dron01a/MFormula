#include "script.h"

mf::unit_vector mf::code_exec(mf::lexer * lex, stack_frame * frame, stop_func func){
	mf::unit_vector result; // результат выполенения программы
	mf::unit_stack opr_buf; // буфер для операций
	mf::unit_stack param_buf; // буфер для параметров операций
	mf::unit temp = lex->next_lex(); // текущая полученная часть кода
	while (func(temp)) {
		if (code_bloc_exec_table.count(temp.type) != 0) {
			code_bloc_exec_table[temp.type](lex, result, frame);
			temp = lex->next_lex();
			continue;
		}
		switch (temp.type) {
		case _type::_end_of_code:
			return result;
		case _type::_bool:
		case _type::_num:
		case _type::_string:
			result.push_back(temp);
			break;
		case _type::_break:
		case _type::_continue:
			throw tr_opr(temp.type);
		case _type::_opr:
			push_token_if(result, opr_buf, temp, [](unit _unit, unit_stack & oprs) {
				return  ((oprs.top().type == _type::_opr && _unit.prior <= oprs.top().prior)
					|| oprs.top().type == _type::_func
					|| oprs.top().type == _type::_core_func
					|| oprs.top().type == _type::_list
					);
			});
			opr_buf.push(temp);
			break;
		case _type::_semicolon:
			push_stack_to_output(result, opr_buf);
			eval(result, frame, param_buf);
			break;
		case _type::_indentf:
			temp.type = frame->get(temp.name, obj_env_type::_obj).type;
			continue;
		}
		temp = lex->next_lex(); // получаем следующюю часть кода
	}
	push_stack_to_output(result, opr_buf);
	eval(result, frame, param_buf);
	return result;
}

void mf::eval(unit_vector & tokens, stack_frame * frame, unit_stack & params) {
	for (size_t count = 0; count < tokens.size(); count++) {
		switch (tokens[count].type) {
		case _type::_list:
		case _type::_var:
		case _type::_const:
		case _type::_type_var:
		case _type::_bool:
		case _type::_num:
		case _type::_string:
		case _type::_self:
			params.push(std::move(tokens[count]));
			break;
		case _type::_opr:
			core_operations[tokens[count].name](params, frame);
			break;
		case _type::_indentf:
			tokens[count].type = frame->get(tokens[count].name, obj_env_type::_obj).type;
			count--;
			break;
		}
	}
	tokens = set_params_from_buf(params, params.size());
}

void mf::var_init_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame){
	init_data_exec(lex, frame, [](lexer * _lex, stack_frame * _frame, unit & _data) {
		_data.type = _type::_var;
		if (_lex->last_lex().name == "=") {
			unit result = calculate_expr(_lex, _frame);
			_data.assign(result);
		}
	});
}

void mf::self_init_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame) {
	init_data_exec(lex, frame, [](lexer * _lex, stack_frame * _frame, unit & _data) {
		if (_lex->last_lex().name == "=") {
			unit result = calculate_expr(_lex, _frame);
			if (result.__mem == nullptr) {
				throw error(result, "Неверный тип");
			}
			_data.__mem = result.__mem;
		}
		_data.type = _type::_self;
	});
}

void mf::const_init_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame){
	init_data_exec(lex, frame, [](lexer * _lex, stack_frame * _frame, unit & _data) {
		if (_lex->last_lex().name == "=") {
			unit result = calculate_expr(_lex, _frame);
			_data.assign(result);
		}
		else {
			throw error(_lex->last_lex(), "Синтаксическая ошибка");
		}
		_data.type = _type::_const;
	});
}

mf::unit mf::calculate_expr(mf::lexer * lex, stack_frame * frame){
	unit result = mf::code_exec(lex, frame, [](unit & _u) { // получаем значение переменной 
		return _u.type != _type::_semicolon && _u.type != _type::_special && _u.type != _type::_close_r_brt;
	})[0];
	return result;
}

void mf::init_data_exec(mf::lexer * lex, stack_frame * frame, void(*init_clb)(mf::lexer *, stack_frame *, unit & data)){
	lex->next_lex(); // получаем текущюю частицу кода
	while (lex->last_lex().type != _type::_semicolon && lex->last_lex().type != _type::_end_of_code
		&& lex->last_lex().type != _type::_close_r_brt) {
		if (frame->have(lex->last_lex().name, obj_env_type::_obj, false)) {
			throw error(lex->last_lex(), "\"" + lex->last_lex().name + "\" уже обьявленно");
		}
		unit new_data(_type::_indentf, lex->last_lex().name); // новая переменная
		lex->next_lex();
		init_clb(lex, frame, new_data);
		frame->add(new_data, mf::obj_env_type::_obj);
		if (lex->last_lex().type == _type::_special) {
			lex->next_lex(); // если встечена запятая то переходим к следующей частице кода
			if (lex->last_lex().type == _type::_var_init
				|| lex->last_lex().type == _type::_self_init
				|| lex->last_lex().type == _type::_const_init) {
				lex->step_back();
				return;
			}
			continue;
		}
		lex->step_back();
	}
}

void mf::func_init_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame){
	stack_frame * local = new stack_frame(frame); // локальный кадр для вычисления параметров
	unit node = lex->next_lex();; // сохраняем функцию 
	node.type = _type::_func;
	node.src = lex;
	node._childs.resize(2);
	lex->next_lex();
	if (lex->last_lex().type != _type::_open_r_brt) {
		throw error(lex->last_lex(), "Синтаксическая ошибка");
	}
	unit_vector params;
	r_brt_exec(lex, params, local);
	lex->next_lex(); // получаем следующюю частицу кода
	node[0]._childs = std::move(local->defined()); // сохраняем объявленные переменные 
	node.stream_pos = lex->last_pos();
	unit * f_pointer = frame->get_func_owerload(node.name, node._childs.size());
	if (f_pointer != nullptr) {
		delete f_pointer;
		throw error(node, "Данная перегрузка уже объявленна");
	}
	lex->next_lex(); // получаем следующюю частицу кода
	if (lex->last_lex().type != _type::_close_c_brt) {
		size_t open_chars = 1;
		while (open_chars != 0) {
			node[1]._childs.push_back(std::move(lex->last_lex()));
			lex->next_lex(); // получаем следующюю частицу кода
			if (lex->last_lex().type == _type::_open_c_brt) { // если встречена открывающая скобка
				open_chars++;
			}
			else if (lex->last_lex().type == _type::_close_c_brt) { // если встречена закрывающая скобка
				open_chars--;
			}
		}
	}
	node[1]._childs.push_back(unit(_type::_end_of_code));
	frame->add(node, mf::obj_env_type::_obj);
	delete local;
}

void mf::c_brt_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame){
	stack_frame * local = new stack_frame(frame); // локальный кадр
	unit_vector res;
	unit_vector val;
	while (lex->last_lex().type != _type::_close_c_brt) {
		val = code_exec(lex, local, [](unit & _u) { // получаем значение
			return _u.type != _type::_special && _u.type != _type::_close_c_brt; 
		});
		if (val.size() == 1) {
			res.push_back(std::move(val[0]));
		}
		else if (val.size() > 1) {
			unit u; 
			u.type = _type::_list;
			u._childs = std::move(val);
			res.push_back(std::move(u));
		}
		if (lex->last_lex().type == _type::_end_of_code) {
			throw error(lex->last_lex(), "Отсутствует закрывающая скобка");
		}
	}
	if (res.size() > 1) {
		unit u;
		u.type = _type::_list;
		u._childs = std::move(res);
		res = { std::move(u) };
	}
	if (res.size() != 0) {
		result.push_back(std::move(res[0]));
	}
	delete local;
}

void mf::s_brt_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame){
	unit_vector res;
	res = code_exec(lex, frame,
		[](unit & _u) { // получаем значение
		return _u.type != _type::_close_s_brt;
	});
	if (res.size() > 1) {
		throw error(lex->last_lex(), "Неверное число параметров");
	}
	if (lex->last_lex().type == _type::_end_of_code) {
		throw error(lex->last_lex(), "Отсутствует закрывающая скобка");
	}
	res = { result.back(), res[0], unit(_type::_opr, "[]") };
	unit_stack us;
	eval(res, frame, us);
	result.back() = res[0];
}

void mf::r_brt_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame){
	unit_vector res;
	while (lex->last_lex().type != _type::_close_r_brt) {
		res = code_exec(lex, frame,
			[](unit & _u) { // получаем значение
			return _u.type != _type::_special && _u.type != _type::_close_r_brt;
		});
		if (res.size() == 1) {
			result.push_back(std::move(res[0]));
		}
		else if (res.size() > 1) {
			result.insert(result.end(), std::move(res.begin()), std::move(res.end()));
		}
		else if (lex->last_lex().type == _type::_end_of_code) {
			throw error(lex->last_lex(), "Отсутствует закрывающая скобка");
		}
	}
}

void mf::get_var_from_memory(mf::lexer * lex, unit_vector & result, stack_frame * frame) {
	result.push_back(lex->last_lex());
	if (result.back().name != "" && result.back().__mem == nullptr) {
		result.back().__mem = &frame->get(result.back().name, obj_env_type::_obj);
	}
}

void mf::get_self_from_memory(mf::lexer * lex, unit_vector & result, stack_frame * frame) {
	result.push_back(std::move(lex->last_lex()));
	result.back().__mem = frame->get(result.back().name, obj_env_type::_obj).__mem;
	if (result.back().__mem != nullptr) {
		result.back().type = result.back().__mem->type;
	}
}

void mf::if_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame){
	lex->next_lex();
	if (lex->last_lex().type != _type::_open_r_brt) { // если не обнаруженно условие
		throw error(lex->last_lex(), "Синтаксическая ошибка, ожидается (");
	}
	unit_vector cond = code_exec(lex, frame, [](unit & _u) { // результат вычисления условия 
		return _u.type != _type::_close_r_brt; 
	}); 
	lex->next_lex();
	if (cond[0].to_bool() && !frame->check_flag(IF_TRUE_COND_FLAG)) {
		unit_vector res;
		c_brt_exec(lex, res, frame); // выполняем тело конструкции ветвления
		lex->next_lex(); // получаем следующюю частицу кода
		frame->set_flag(IF_TRUE_COND_FLAG);
	}
	else {
		skip_brt(lex, _type::_open_c_brt, _type::_close_c_brt); // пропускаем область видимости if
		if (!frame->check_flag(IF_TRUE_COND_FLAG)) { 
			frame->set_flag(IF_FALSE_COND_FLAG);
		};
	}
	if(lex->last_lex().type != _type::_else) {
		frame->reset_flag(IF_TRUE_COND_FLAG | IF_FALSE_COND_FLAG);
	}
	lex->step_back(); // возвращаемся на одну позицию назад
}

void mf::else_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame){
	lex->next_lex(); // получаем следующюю частицу кода
	if (frame->check_flag(IF_FALSE_COND_FLAG)) {
		if (lex->last_lex().type == _type::_if) {
			frame->reset_flag(IF_TRUE_COND_FLAG | IF_FALSE_COND_FLAG);
			if_exec(lex, result, frame);
			return;
		}
		c_brt_exec(lex, result, frame); // выполняем тело конструкции ветвления
	}
	if (frame->check_flag(IF_TRUE_COND_FLAG)) {
		if (lex->last_lex().type == _type::_if) {
			if_exec(lex, result, frame);
			return;
		}
		skip_brt(lex, _type::_open_c_brt, _type::_close_c_brt); // пропускаем область видимости else
		lex->step_back();
	}
	frame->reset_flag(IF_TRUE_COND_FLAG | IF_FALSE_COND_FLAG);
}

void mf::switch_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame){
	lex->next_lex();
	if (lex->last_lex().type != _type::_open_r_brt) { // если не обнаруженно условие
		throw error(lex->last_lex(), "Синтаксическая ошибка, ожидается (");
	} 
	unit_vector cond(2);
	cond[0] = code_exec(lex, frame, [](unit & _u) { // результат вычисления условия 
		return _u.type != _type::_close_r_brt;
	})[0];
	lex->next_lex();
	if (lex->last_lex().type != _type::_open_c_brt) { // если не обнаруженна {
		throw error(lex->last_lex(), "Синтаксическая ошибка, ожидается {");
	}
	lex->next_lex();
	while (lex->last_lex().type != _type::_close_c_brt){
		if (lex->last_lex().type != _type::_case && lex->last_lex().type != _type::_default) { 
			throw error(lex->last_lex(), "Синтаксическая ошибка, ожидается case или default"); // если не обнаружен case
		}
		switch (lex->last_lex().type){
		case _type::_case:
			if (!frame->check_flag(SWITCH_TRUE_RESULT_FLAG)) {
				cond[1] = code_exec(lex, frame, [](unit & _u) { // результат вычисления условия 
					return _u.type != _type::_colon;
				})[0];
			}
			else {
				lex->skip_to(_type::_colon);
			}
			if (value(cond[0]) == value(cond[1]) || frame->check_flag(SWITCH_NOT_BREAK_FLAG)) {
				code_exec(lex, frame, [](unit & _u) {
					return _u.type != _type::_break && _u.type != _type::_case;
				});
				if (!frame->check_flag(SWITCH_NOT_BREAK_FLAG)) {
					frame->set_flag(SWITCH_TRUE_RESULT_FLAG);
				}
				if (lex->last_lex().type == _type::_break) {
					lex->next_lex();
					frame->reset_flag(SWITCH_NOT_BREAK_FLAG);
					lex->skip_to([](_type _t) {
						return _t != _type::_case && _t != _type::_close_c_brt && _t != _type::_default;
					});
				}
				if (lex->last_lex().type == _type::_case) {
					frame->set_flag(SWITCH_NOT_BREAK_FLAG);
					lex->step_back();
				}
			}
			else {
				while (lex->last_lex().type != _type::_close_c_brt && 
					lex->last_lex().type != _type::_case && lex->last_lex().type != _type::_default) {
					if (lex->last_lex().type == _type::_open_c_brt) {
						lex->skip_brt(_type::_open_c_brt, _type::_close_c_brt);
					}
					lex->next_lex();
				}
			}
			break;
		case _type::_default:
			if (!frame->check_flag(SWITCH_TRUE_RESULT_FLAG) || frame->check_flag(SWITCH_NOT_BREAK_FLAG)) {
				lex->next_lex();
				code_exec(lex, frame, [](unit & _u) {
					return _u.type != _type::_break && _u.type != _type::_case && _u.type != _type::_close_c_brt;
				});
				if (lex->last_lex().type == _type::_break) {
					lex->next_lex();
					frame->reset_flag(SWITCH_NOT_BREAK_FLAG);
					lex->skip_to([](_type _t) {
						return _t != _type::_case && _t != _type::_close_c_brt && _t != _type::_default;
					});
				}
				if (lex->last_lex().type == _type::_case) {
					frame->set_flag(SWITCH_NOT_BREAK_FLAG);
					lex->step_back();
				}
			}
			else {
				while (lex->last_lex().type != _type::_close_c_brt &&
					lex->last_lex().type != _type::_case && lex->last_lex().type != _type::_default) {
					if (lex->last_lex().type == _type::_open_c_brt) {
						lex->skip_brt(_type::_open_c_brt, _type::_close_c_brt);
					}
					lex->next_lex();
				}
			}
			break;
		}
	}
	frame->reset_flag(SWITCH_TRUE_RESULT_FLAG | SWITCH_NOT_BREAK_FLAG);
}

void mf::while_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame){
	lex->next_lex(); // получаем следующюю частицу кода
	if (lex->last_lex().type != _type::_open_r_brt) {
		throw error(lex->last_lex(), "Синтаксическая ошибка");
	}
	stack_frame * local = new stack_frame(frame); // локальный кадр
	mf::cached_lexer * cond_cach = new mf::cached_lexer(""); // кеширование условия
	caching_expr_brt(cond_cach, lex, _type::_open_r_brt, _type::_close_r_brt);
	cond_cach->src_frame = local;
	lex->next_lex();
	mf::cached_lexer * body_cach = new mf::cached_lexer(""); // кеширование тела цикла 
	caching_expr_brt(body_cach, lex, _type::_open_c_brt, _type::_close_c_brt);
	unit_vector cond = code_exec(cond_cach, local, [](unit & _u) { return _u.type != _type::_semicolon; });
	while (cond[0].to_bool()) {
		cond[0] = std::move(loop_body_exec(body_cach, cond_cach, local));
	}
	delete body_cach;
	delete cond_cach;
	delete local;
	lex->src_frame = frame; // ???
}

void mf::for_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame){
	lex->next_lex(); // получаем следующюю частицу кода
	if (lex->last_lex().type != _type::_open_r_brt) {
		throw error(lex->last_lex(), "Синтаксическая ошибка");
	}
	stack_frame * local = new stack_frame(frame); // локальный кадр
	lex->src_frame = local;
	code_exec(lex, local, [](unit & _u) { return _u.type != _type::_semicolon; });
	mf::cached_lexer * cond_cach = new mf::cached_lexer(""); // кеширование условия
	cond_cach->src_frame = local;
	do { 
		lex->next_lex(); // получаем следующюю частицу кода
		cond_cach->add(lex->last_lex());
	} while (lex->last_lex().type != _type::_semicolon);
	mf::cached_lexer * step_cach = new mf::cached_lexer(""); // кешировние шага
	step_cach->src_frame = local;
	while (lex->last_lex().type != _type::_open_c_brt) { 
		lex->next_lex();
		step_cach->add(lex->last_lex()); // получаем следующюю частицу кода
	}
	mf::cached_lexer * body_cach = new mf::cached_lexer(""); // кеширование тела цикла 
	caching_expr_brt(body_cach, lex, _type::_open_c_brt, _type::_close_c_brt);
	unit_vector cond = code_exec(cond_cach, local, [](unit & _u) { return _u.type != _type::_semicolon; });
	while(cond[0].to_bool()) {
		cond[0] = std::move(loop_body_exec(body_cach, cond_cach, local, step_cach));
	}
	delete body_cach;
	delete cond_cach;
	delete step_cach;
	delete local;
	lex->src_frame = frame;
}

mf::unit mf::loop_body_exec(mf::lexer * body_lex, mf::lexer * condition_lex, stack_frame * frame, mf::lexer * step){
	mf::unit result(_type::_bool);
	stack_frame * local = new stack_frame(frame); // локальный кадр выполенения тела цикла
	try {
		body_lex->set_pos(0); // выполнение тела цикла
		body_lex->src_frame = local;
		code_exec(body_lex, local, [](unit & u) { return u.type != _type::_end_of_code; });
	}
	catch (tr_opr _opr) {
		switch (_opr.type) {
		case _type::_break: 
			result.bool_val = false;
			goto out_body; 
		case _type::_continue: 
			goto result_comp;
		default:
			throw _opr; // если был пойман return
		}
	}
	condition_lex->set_pos(0);
result_comp:
	if (step != nullptr) {
		step->set_pos(0);
		code_exec(step, local, [](unit & u) { return u.type != _type::_open_c_brt; });
	}
	result = code_exec(condition_lex, local, [](unit & _u) {
		 return _u.type != _type::_end_of_code && _u.type != _type::_semicolon; 
	 })[0];
 out_body:
	delete local;
	return result;
}

void mf::call_func_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame) {
	stack_frame * local = new stack_frame(frame); // локальный кадр 
	unit * func = new mf::unit(); // указатель на вызываемую функцию
	func->name = lex->last_lex().name;       
	lex->next_lex();
	unit_vector params = code_exec(lex, local, [](unit & _u) { // получение параметров функции
		return _u.type != _type::_close_r_brt; 
	});
	func = local->get_func_owerload(func->name, params.size());
	if (func == nullptr) {
		throw error(*func, "Неверное колличество параметров функции");
	}
	local->defined() = func->_childs[0]._childs;
	for (size_t count = 0; count < params.size(); ++count) {
		switch (func->_childs[0]._childs[count].type) { // устанавливаем значения параметров
		case _type::_self:
			local->defined()[count].__mem = params[count].__mem;
			break;
		default:
			local->defined()[count].assign(original(params[count]));
			break;
		}
	}
	mf::cached_lexer * body = new mf::cached_lexer("");
	try {
		if (func->_childs[1]._childs.size() > 0) {
			body->add(func->_childs[1]._childs);
			body->src_frame = local;
			code_exec(body, local, [](unit & _u) { return _u.type != _type::_end_of_code; });
		}
	}
	catch (tr_opr & _T) {
		if (_T.type != _type::_return) {
			throw _T;
		}
		result.push_back(std::move(_T.value)); // устанавливаем возвращаемое значение
	}
	func = nullptr; // освобождаем выделенную память
	delete body;
	delete func;
	delete local;
}

void mf::core_func_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame){
	unit node = lex->last_lex();
	lex->next_lex(); // получаем следующюю частицу кода
	if (lex->last_lex().type != _type::_open_r_brt) {
		throw error(lex->last_lex(), "Синтаксическая ошибка");
	}
	unit_vector params; // параметры функции
	r_brt_exec(lex, params, frame); // получение параметров функции
	node._childs = std::move(params);
	unit_stack par_st;
	core_fcn[node.name](node, par_st, frame);
	if (par_st.size() != 0) {
		result.push_back(std::move(par_st.top()));
	}
}

void mf::return_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame){
	unit_vector res = code_exec(lex, frame, [](unit & _u) { return _u.type != _type::_semicolon; });
	throw tr_opr(_type::_return, value(res[0]));
}

void mf::type_init_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame){
	lex->next_lex();
	if (lex->last_lex().type != _type::_indentf) { // проверка на ошибки
		throw error(lex->last_lex(), "Синтаксическая ошибка"); 
	}
	else if(frame->have(lex->last_lex().name, obj_env_type::_type, true)) { // проверяем тип на существование 
		throw error(lex->last_lex(), "тип \"" + lex->last_lex().name + "\" уже существует");
	}
	mf::unit new_type = lex->last_lex();
	new_type.type = _type::_type;
	lex->next_lex();
	if (lex->last_lex().type != _type::_open_c_brt) {
		throw error(lex->last_lex(), "Синтаксическая ошибка");
	}
	stack_frame *  init_result = new stack_frame();; // хранилище для результата инициализации типа
	mf::code_exec(lex, init_result, [](unit & _u) { return _u.type != _type::_close_c_brt; }); // выполняем код внутри
	mf::unit_vector type_constructs; //= stack_frame.get_all_of(new_type.name); // получаем все доступные конструкторы
	for (size_t i = 0; i < init_result->defined().size(); ++i) {
		if (init_result->defined()[i].name == new_type.name) {
			init_result->defined()[i].type = _type::_type_constr;
			type_constructs.push_back(init_result->defined()[i]);
		}
	}
	switch (type_constructs.size()){
	case 1:
		frame->defined().push_back(std::move(type_constructs[0])); // если конструктор всего один 
		break;
	case 0:
		frame->defined().push_back(mf::unit(_type::_type_constr, new_type.name)); // если конструктор отсутствует 
		frame->defined().back()._childs.resize(2);
	default:
		frame->defined().insert(frame->defined().end(), std::move(type_constructs.begin()), std::move(type_constructs.end()));
		break;
	}
	new_type._childs = std::move(init_result->defined()); // сохраняем инициализированные поля типа
	frame->add(new_type, obj_env_type::_type); // добавляем новый тип в среду выполениния
}

void mf::type_constr_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame){
	mf::unit res; // результат
	stack_frame * local = new stack_frame(frame); // локальный кадр 
	local->defined() = frame->get(lex->last_lex().name, obj_env_type::_type)._childs;
	call_func_exec(lex, result, local); // выполенение функции конструктора
	res.type = _type::_type_var;
	res._childs = std::move(local->defined());
	result.push_back(std::move(res));
	delete local;
	//lex->step_back();
}

void mf::dot_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame){
	if (original(result.back()).type != _type::_type_var) {
		throw mf::error(lex->last_lex(), "Синтаксическая ошибка");
	}
	mf::unit_vector & fields = mf::original(result.back())._childs; // поля 
	mf::unit * type_field = nullptr; // ссылка на поле типа
	lex->next_lex();
	for (size_t i = 0; i < fields.size(); ++i) {
		if (fields[i].name == lex->last_lex().name) {
			type_field = &fields[i];
			break;
		}
	}
	if (type_field == nullptr) {
		throw mf::error(lex->last_lex(), "Не содержит члена " + lex->last_lex().name);
	}
	if (type_field->type == _type::_var) {
		result.back() = *type_field;
		result.back().__mem = type_field;
	}
	else if (type_field->type == _type::_func) {
		result.pop_back();
		stack_frame * local = new stack_frame(frame); // локальный кадр для выполнения для метода
		local->defined() = fields; // копируем поля
		call_func_exec(lex, result, local); // выполенение 
		fields = std::move(local->defined());
		delete local;
	}
}

void mf::include_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame){
	unit_vector res = code_exec(lex, frame, [](mf::unit & _u) { return _u.type != _type::_semicolon; });
	if (res.size() == 0) {
		throw error(lex->last_lex(), "Требуется аргумент справа");
	}
	if (res[0].type != _type::_string) {
		throw error(res[0], "Неправильный тип");
	}
	mf::file_lexer * inc_lex = new mf::file_lexer(res[0].name); // выполняем включаемый файл
	code_exec(inc_lex, frame, [](mf::unit & u) { return u.type != mf::_type::_end_of_code; });
	delete inc_lex;
}

void mf::check_num_of_func_params(unit & func, size_t count) {
    if(func._childs.size() != count){
        throw error(func, "Неверное колличество параметров");
    }
}

mf::unit_vector mf::set_params_from_buf(unit_stack & buff, int _count){
	if (buff.size() < _count) {
		throw error(buff.top(), "Неверно указанное число параметров!");
	}
	unit_vector result;
	result.resize(_count);
	for (int i = _count; i != 0; i--) {
		result[i - 1] = std::move(buff.top());
		buff.pop();
	}
	return result;
}

void mf::set_in_env(unit_stack & params, stack_frame * frame) {
	unit_vector _params = set_params_from_buf(params, 2); // вектор с параметрами 
	unit _temp;
	if (_params[0].__mem != nullptr) {
		_temp.__mem = &_params[0].__mem->_childs[original(_params[1]).to_int()];
		_temp.type = _temp.__mem->type;
	}
	else if (frame->have(_params[0].name, obj_env_type::_obj)) {
		_temp.__mem = &frame->get(_params[0].name, obj_env_type::_obj)._childs[original(_params[1]).to_int()];
		_temp.type = _temp.__mem->type;
	}
	else {
		_temp = _params[0]._childs[original(_params[1]).to_int()];
	}
	params.push(_temp);
}

void mf::assign(unit_stack & _params, stack_frame * frame){
    unit_vector params = std::move(set_params_from_buf(_params, 2));
	switch (original(params[0]).type){
	case _type::_const:
		throw error(params[0], "Для констант не доступна операция присвоения");
	case _type::_self:
		frame->get(original(params[0]).name, obj_env_type::_obj).__mem = params[1].__mem;
		break;
	default:
		original(params[0]).assign(original(params[1]));
		break;
	}
}

void mf::decrem(unit_stack & params, stack_frame * frame){
    if(original(params.top()).type == _type::_var ){
        original(params.top())[0] = std::move(original(params.top())[0].decrement());
    }
    if(original(params.top()).type == _type::_num ){
        original(params.top()) = std::move(original(params.top()).decrement());
    }
    params.pop();
}

void mf::increm(unit_stack & params, stack_frame * frame){
    if(original(params.top()).type == _type::_var ){
        original(params.top())[0] = std::move(original(params.top())[0].increment());
    }
    if(original(params.top()).type == _type::_num ){
        original(params.top()) = std::move(original(params.top()).increment());
    }
    params.pop();
}

void mf::caching_expr_brt(cached_lexer * cl, lexer * src, _type open_brt, _type close_brt){
	size_t open_chars = 1;
	src->next_lex();
	while (open_chars != 0) {
		cl->add(src->last_lex());
		src->next_lex(); // получаем следующюю частицу кода
		if (src->last_lex().type == open_brt) { // если встречена открывающая скобка
			open_chars++;
		}
		else if (src->last_lex().type == close_brt) { // если встречена закрывающая скобка
			open_chars--;
		}
	}
	unit endc = unit(_type::_end_of_code, "");
	cl->add(endc);
}

void mf::skip_brt(lexer * lex, _type open_brt, _type close_brt){
	size_t open_chars = 1;
	lex->next_lex();
	while (open_chars != 0) {
		if (lex->last_lex().type == open_brt) { // если встречена открывающая скобка
			open_chars++;
		}
		else if (lex->last_lex().type == close_brt) { // если встречена закрывающая скобка
			open_chars--;
		}
		lex->next_lex(); // получаем следующюю частицу кода
	}
}

void mf::push_token_if(unit_vector & _tokens, unit_stack & opr, unit token, cond_func func) {
	while (opr.size() && func(token, opr)) {
		_tokens.push_back(std::move(opr.top()));
		opr.pop();
	}
}

void mf::push_stack_to_output(unit_vector & _tokens, unit_stack & opr) {
	while (opr.size()) {
		_tokens.push_back(std::move(opr.top()));
		opr.pop();
	}
}
