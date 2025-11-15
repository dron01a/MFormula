#ifndef _SCRIPT_H_
#define _SCRIPT_H_

#include <fstream>
//#include "parse.h"
#include "unit.h"
#include "lexer.h"
#include "stack.h"

#include "math.h"

#define PI 3.1415926535

namespace mf {

	typedef void(*code_block_exec)(mf::lexer *, unit_vector &, stack_frame *);// функция обработки блоков кода
	typedef bool(*stop_func)(unit &);// условие остановки выполенения
	typedef bool(*cond_func)(unit, unit_stack &); // перемещает элементы из стека в соответствии с условием
	typedef void(*core_operation)(unit_stack &,  stack_frame * );
	typedef void(*core_func)(unit &, unit_stack &, stack_frame *);

	// функция выполеления исходного кода
	mf::unit_vector code_exec(mf::lexer * lex, stack_frame * frame, stop_func func);

	// выполение выражений
	void eval(unit_vector & tokens, stack_frame * frame, unit_stack & params);

	// объявление переменных 
	void var_init_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame);

	// объявление функции
	void func_init_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame);

	// объявление ссылки
	void self_init_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame);

	// объявление константы
	void const_init_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame);

	// вычисление выражения домоемента пока не будут встречаны знаки , ; )
	unit calculate_expr(mf::lexer * lex, stack_frame * frame);

	// базовая функция для инициализации констант, переменных и ссылок
	void init_data_exec(mf::lexer * lex, stack_frame * frame, void (*init_clb)(lexer *, stack_frame *, unit & data));

	// расчет элементов в фигурных скобках 
	void c_brt_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame);

	// расчет элементов в квадратных скобках 
	void s_brt_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame);

	// расчет элементов в круглых скобках 
	void r_brt_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame);

	// получает объявленную переменную, объект из памяти
	void get_var_from_memory(mf::lexer * lex, unit_vector & result, stack_frame * frame);
	
	// получает объявленную ссылку из памяти
	void get_self_from_memory(mf::lexer * lex, unit_vector & result, stack_frame * frame);

	// выполенение блока if
	void if_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame);

	// выполенение блока else
	void else_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame);

	// выполенение switch
	void switch_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame);

	// выполнение цикла while
	void while_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame);

	// выполнение цикла for
	void for_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame);

	// выполенение тела цикла
	mf::unit loop_body_exec(mf::lexer * body_lex, mf::lexer * condition_lex, stack_frame * frame, mf::lexer * step = 0);
	
	// выполнение вызова функции
	void call_func_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame);

	// выполенение встроенных функций
	void core_func_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame);

	// выполнение иснтрукции return
	void return_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame);

	// выполенние инициализации нового пользовательского типа
	void type_init_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame);
	
	// вызов конструктора типа
	void type_constr_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame);

	// выполенения оператора получения доступа к элементу объекта
	void dot_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame);

	// выполенение дерективы include
	void include_exec(mf::lexer * lex, unit_vector & result, stack_frame * frame);

	// парсинг единицы кода с условием
	void push_token_if(unit_vector & _tokens, unit_stack & opr, unit token, cond_func func);

	// помещает стек с операциями в выходной вектор
	void push_stack_to_output(unit_vector & _tokens, unit_stack & opr);

	// проверяет колличесвто парметров у вызываемой функции
	void check_num_of_func_params(unit & func, size_t count);

	// возвращает вектор с параметрами оператора или функции из буфера
	unit_vector set_params_from_buf(unit_stack & buff, int _count);

	// устанавливает устанавливает знаения из среды выполения в стек с параметрами
	void set_in_env(unit_stack & params, stack_frame * frame);

	// присвоение
	void assign(unit_stack & _params, stack_frame * frame);
	
	// дикремент
	void decrem(unit_stack & params, stack_frame * frame);
	
	// инкремент
	void increm(unit_stack & params, stack_frame * frame);

	// кеширование выражения 
	void caching_expr_brt(cached_lexer * cl, lexer * src, _type open_brt, _type close_brt);

	// пропускает скобки
	void skip_brt(lexer * lex, _type open_brt, _type close_brt);

	// структура для операторов перехода break, continue, return
	struct tr_opr{
		tr_opr( _type _t){
	        type = _t;
	    } 
		tr_opr(_type _t, unit val) {
			type = _t;
			value = val;
		}
	    ~tr_opr(){};
	    _type type;
		unit value;
	};

	// таблица с блоками кода и их обратными вызовами
	static std::map<_type, code_block_exec> code_bloc_exec_table{
		{ _type::_var_init, var_init_exec },
		{ _type::_self_init, self_init_exec },
		{ _type::_const_init, const_init_exec },
		{ _type::_function_init, func_init_exec },
		{ _type::_open_s_brt, s_brt_exec },
		{ _type::_open_r_brt, r_brt_exec },
		{ _type::_open_c_brt, c_brt_exec },
		{ _type::_var, get_var_from_memory },
		{ _type::_const, get_var_from_memory },
		{ _type::_list, get_var_from_memory },
		{ _type::_type_var, get_var_from_memory },
		{ _type::_self, get_self_from_memory },
		{ _type::_if, if_exec },
		{ _type::_else, else_exec },
		{ _type::_while, while_exec }, 
		{ _type::_for, for_exec }, 
		{ _type::_core_func, core_func_exec }, 
		{ _type::_func, call_func_exec },
		{ _type::_return, return_exec }, 
		{ _type::_type_init, type_init_exec },
		{ _type::_type_constr, type_constr_exec },
		{ _type::_type_field_call_opr, dot_exec }, 
		{ _type::_include, include_exec},
		{ _type::_switch, switch_exec }
	};

	static std::map<std::string, core_operation> core_operations{
		{ "==",[](std::stack<unit> & params, stack_frame * frame) {
			unit_vector _params = set_params_from_buf(params, 2);
			params.push(unit(original(_params[0]) == original(_params[1])));
		}},
		{ "!=",[](std::stack<unit> & params, stack_frame * frame) {
			unit_vector _params = set_params_from_buf(params, 2);
			params.push(unit(original(_params[0]) != original(_params[1])));
		}},
		{ ">",[](std::stack<unit> & params, stack_frame * frame) {
			unit_vector _params = set_params_from_buf(params, 2);
			params.push(unit(original(_params[0]) > original(_params[1])));
		}},
		{ "<",[](std::stack<unit> & params, stack_frame * frame) {
			unit_vector _params = set_params_from_buf(params, 2);
			params.push(unit(original(_params[0]) < original(_params[1])));
		} },
		{ ">=",[](std::stack<unit> & params, stack_frame * frame) {
			unit_vector _params = set_params_from_buf(params, 2);
			params.push(unit(original(_params[0]) >= original(_params[1])));
		} },
		{ "<=",[](std::stack<unit> & params, stack_frame * frame) {
			unit_vector _params = set_params_from_buf(params, 2);
			params.push(unit(original(_params[0]) <= original(_params[1])));
		} },
		{ "&&",[](std::stack<unit> & params, stack_frame * frame) {
			unit_vector _params = set_params_from_buf(params, 2);
			params.push(unit(original(_params[0]) && original(_params[1])));
		} },
		{ "||",[](std::stack<unit> & params, stack_frame * frame) {
			unit_vector _params = set_params_from_buf(params, 2);
			params.push(unit(original(_params[0]) || original(_params[1])));
		} },
		{ "+",[](std::stack<unit> & params, stack_frame * frame) {
			unit_vector _params = set_params_from_buf(params, 2);
			params.push(unit(original(_params[0]) + original(_params[1])));
		} },
		{ "-",[](std::stack<unit> & params, stack_frame * frame) {
			unit_vector _params = set_params_from_buf(params, 2);
			params.push(unit(original(_params[0]) - original(_params[1])));
		} },
		{ "/",[](std::stack<unit> & params, stack_frame * frame) {
			unit_vector _params = set_params_from_buf(params, 2);
			params.push(unit(original(_params[0]) / original(_params[1])));
		} },
		{ "*",[](std::stack<unit> & params, stack_frame * frame) {
			unit_vector _params = set_params_from_buf(params, 2);
			params.push(unit(original(_params[0]) * original(_params[1])));
		} },
		{ "%",[](std::stack<unit> & params, stack_frame * frame) {
			unit_vector _params = set_params_from_buf(params, 2);
			params.push(unit(original(_params[0]) % original(_params[1])));
		} },
		{ "^",[](std::stack<unit> & params, stack_frame * frame) {
			unit_vector _params = set_params_from_buf(params, 2);
			params.push(unit(original(_params[0]) ^ original(_params[1])));
		} },
	    {"[]", set_in_env},
	    {"=", assign },
	    {"++", increm },
	    {"--", decrem },
	    { "+=", [](std::stack<unit> & params, stack_frame * frame){
	        unit_vector _params = set_params_from_buf(params, 2);
	        original(_params[0]) += original(_params[1]);
	    }},
	    {"-=",[](std::stack<unit> & params, stack_frame * frame){
	        unit_vector _params = set_params_from_buf(params, 2);
	        original(_params[0]) -= original(_params[1]);
	    }},    
	    {"/=",[](std::stack<unit> & params, stack_frame * frame){
	        unit_vector _params = set_params_from_buf(params, 2);
	        original(_params[0]) /= original(_params[1]);
	    }},    
	    {"*=",[](std::stack<unit> & params, stack_frame * frame){
	        unit_vector _params = set_params_from_buf(params, 2);
	        original(_params[0]) *= original(_params[1]);
	    }},
	    {"%=",[](std::stack<unit> & params, stack_frame * frame){
	        unit_vector _params = set_params_from_buf(params, 2);
	        original(_params[0]) %= original(_params[1]);
	    }},
	    {"^=",[](std::stack<unit> & params, stack_frame * frame){
	        unit_vector _params = set_params_from_buf(params, 2);
	        original(_params[0]) ^= original(_params[1]);
	    }},
		{ "~",[](std::stack<unit> & params, stack_frame * frame) {
			params.top() = unit(-params.top().to_double());
		}},
	};

	static std::map<std::string, core_func> core_fcn{
		{"get_type", [](unit & node, std::stack<unit> & params, stack_frame * frame) {
			check_num_of_func_params(node, 1);
			switch (node[0].type) {
			case _type::_var:
				node._childs[0] = original(node._childs[0])[0];
				core_fcn["get_type"](node, params, frame);
				break;
			case _type::_bool:
				params.push(unit(_type::_string, "boolean"));
				break;
			case _type::_num:
				params.push(unit(_type::_string, "number"));
				break;
			case _type::_string:
				params.push(unit(_type::_string, "string"));
				break;
			case _type::_list:
				params.push(unit(_type::_string, "list"));
				break;
			case _type::_type_var:
				{
				auto f = std::find_if(node[0]._childs.begin(), node[0]._childs.end(), [](unit & u) { return u.type == _type::_type_constr; });
				params.push(unit(_type::_string, f->name));
				}
				break;
			}
		}},
		{"delete",  [](unit & node, std::stack<unit> & params, stack_frame * frame) {
			while (!node._childs.empty()) {
				switch (node._childs.back().type) {
				case _type::_var:
				case _type::_list:
				case _type::_self:
					frame->delete_var(node._childs.back().name);
					node._childs.pop_back();
					break;
				default:
					throw error(node._childs.back(), "error type");
				}
			}
		}},
		{"defined", [](unit & node, std::stack<unit> & params, stack_frame * frame) {
			check_num_of_func_params(node, 0);
			for (size_t i = 0; i < frame->defined().size(); i++) {
				if (original(frame->defined()[i]).type == _type::_list) {
					std::cout << frame->defined()[i].name << " => " << frame->get(original(frame->defined()[i]).name, obj_env_type::_obj) << std::endl;
					continue;
				}
				if (original(frame->defined()[i]).type == _type::_func) {
					std::cout << frame->defined()[i].name << " => func\n";
					continue;
				}
				std::cout << frame->defined()[i].name << " => " << original(frame->defined()[i]) << std::endl;
			}
			if (frame->get_perv() != nullptr) {
				core_fcn["defined"](node, params, frame->get_perv());
			}
		}},
		{"print", [](unit & node, std::stack<unit> & params, stack_frame * frame) {
			for (size_t _par = 0; _par < node._childs.size(); _par++) {
				if (original(node._childs[_par]).type == _type::_list) {
					std::cout << original(node._childs[_par]);
					continue;
				}
				std::cout << original(node._childs[_par]);
			}
			std::cout << std::endl;
		}},
		{"size", [](unit & node, std::stack<unit> & params, stack_frame * frame) {
			check_num_of_func_params(node, 1);
			params.push(unit((long double)original(node._childs[0]).size()));
		}},
		{"resize", [](unit & node, std::stack<unit> & params, stack_frame * frame) {
			check_num_of_func_params(node, 2);
			original(node._childs[0]).resize(original(node._childs[1]).to_int());
		}},
		{"push_back", [](unit & node, std::stack<unit> & params, stack_frame * frame) {
			check_num_of_func_params(node, 2);
			frame->get(node._childs[0].name, obj_env_type::_obj)._childs.push_back(value(node._childs[1]));
		}},
		{"run", [](unit & node, std::stack<unit> & params, stack_frame * frame) {
			check_num_of_func_params(node, 1);
			if (node._childs[0].type != _type::_string) {
				throw error(node._childs[0], "error type!");
			}
			/* unit_vector _subcode = lex_file(node._childs[0].to_string(), env);
			 run_script(_subcode, env);*/
		 }},
		 {"system", [](unit & node, std::stack<unit> & params, stack_frame * frame) {
			 check_num_of_func_params(node, 1);
			 if (node._childs[0].type != _type::_string) {
				 throw error(node._childs[0], "error type!");
			 }
			 std::system(node._childs[0].to_string().c_str());
		 }},
		 {"exit", [](unit &, std::stack<unit> & params, stack_frame * frame) {
			 exit(0);
		 }},
		 {"read",[](unit & node, std::stack<unit> & params, stack_frame * frame) {
			 for (size_t i = 0; i < node._childs.size(); i++) {
				 unit _temp;
				 std::cin >> _temp;
				 original(node._childs[i]).assign(_temp);
			 }
		 }},
		 {"clear", [](unit & node, std::stack<unit> & params, stack_frame * frame) {
			 check_num_of_func_params(node, 0);
			 std::cout << "\x1B[2J\x1B[H";
		 }},
		 {"sin",[](unit & node, std::stack<unit> & params, stack_frame * frame) {
			 check_num_of_func_params(node, 1);
			 params.push(unit(sin(node._childs[0].to_double())));
		 }},
		 {"cos",[](unit & node, std::stack<unit> & params, stack_frame * frame) {
			 check_num_of_func_params(node, 1);
			 params.push(unit(cos(node._childs[0].to_double())));
		 }},
		 {"tg",[](unit & node, std::stack<unit> & params, stack_frame * frame) {
			 check_num_of_func_params(node, 1);
			 params.push(unit(tan(node._childs[0].to_double())));
		 }},
		 {"ctg",[](unit & node, std::stack<unit> & params, stack_frame * frame) {
			 check_num_of_func_params(node, 1);
			 params.push(unit(1 / tan(node._childs[0].to_double())));
		 }},
		 {"arcsin",[](unit & node, std::stack<unit> & params, stack_frame * frame) {
			 check_num_of_func_params(node, 1);
			 params.push(unit(asin(node._childs[0].to_double())));
		 }},
		 {"arccos",[](unit & node, std::stack<unit> & params, stack_frame * frame) {
			 check_num_of_func_params(node, 1);
			 params.push(unit(acos(node._childs[0].to_double())));
		 }},
		 {"arctg",[](unit & node, std::stack<unit> & params, stack_frame * frame) {
			 check_num_of_func_params(node, 1);
			 params.push(unit(atan(node._childs[0].to_double())));
		 }},
		 {"arcctg",[](unit & node, std::stack<unit> & params, stack_frame * frame) {
			 check_num_of_func_params(node, 1);
			 params.push(unit(atan(-node._childs[0].to_double()*PI / 2)));
		 }},
		 {"abs",[](unit & node, std::stack<unit> & params, stack_frame * frame) {
			 check_num_of_func_params(node, 1);
			 params.push(unit(abs(node._childs[0].to_double())));
		 }},
		 {"ln",[](unit & node, std::stack<unit> & params, stack_frame * frame) {
			 check_num_of_func_params(node, 1);
			 params.push(unit(log(node._childs[0].to_double())));
		 }},
		 {"deg",[](unit & node, std::stack<unit> & params, stack_frame * frame) {
			 check_num_of_func_params(node, 1);
			 params.push(unit((node._childs[0].to_double()*PI / 180)));
		 }},
		 {"sqrt",[](unit & node, std::stack<unit> & params, stack_frame * frame) {
			 check_num_of_func_params(node, 1);
			 params.push(unit(pow(node._childs[0].to_double(),(1 / 2))));
		 }},
		 {"!",[](unit & node, std::stack<unit> & params, stack_frame * frame) {
			 check_num_of_func_params(node, 1);
			 switch (node._childs[0].type) {
			 case _type::_bool:
				 params.push(unit(!node._childs[0].to_bool()));
				 break;
			 case _type::_var:
				 node[0] = node[0][0];
				 core_fcn["!"](node, params, frame);
				 break;
			 default:
				 throw error(node, "error type");
			 }
		 }},
		 {"log",[](unit & node, std::stack<unit> & params, stack_frame * frame) {
			 check_num_of_func_params(node, 2);
			 params.push(unit((log(node._childs[0].to_double()) / log(node._childs[1].to_double()))));
		 }},
		 {"round",[](unit & node, std::stack<unit> & params, stack_frame * frame) {
			 check_num_of_func_params(node, 2);
			 params.push(unit(round(node._childs[0].to_double()*pow(10,node._childs[1].to_double())) / pow(10,node._childs[1].to_double())));
		 }},
		 { "error",[](unit & node, std::stack<unit> & params, stack_frame * frame) {
			 throw node[0].name;
		 }},
	};
};
#endif

//                  __________        __________
//                  \--------/ <\  /> \--------/
//                   \-------\__/  \__/-------/
//                     \------\      /------/
//                             /\  /\
//							  "' \/ '"
//                      |ИМПЕРАТОР ЗАЩИТИТ|  
//                      |СЛАВА БОГУ МАШИНЕ|