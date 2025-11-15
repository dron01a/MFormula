#include "stack.h"

mf::stack_frame::stack_frame(lexer * lex){
	this->src = lex;
}

mf::stack_frame::stack_frame(stack_frame * perv){
	this->perv = perv;
}

mf::stack_frame::stack_frame(lexer & lex, stack_frame & perv){
	this->src = &lex;
	this->perv = &perv;
}

mf::unit_vector & mf::stack_frame::defined(){
	return _defined;
}

mf::unit & mf::stack_frame::get(std::string _name, obj_env_type type){
	switch (type) {
	case obj_env_type::_obj:
		return get(_defined, _name, type); // поиск переменных и функций
	case obj_env_type::_type:
		return get(types, _name, type); // поиск типов 
	default:
		throw std::string(_name + " -> неизвестный элемент").c_str();
	}
}

mf::unit * mf::stack_frame::get_func_owerload(std::string name, size_t par_count){
	for (size_t i = 0; i < _defined.size(); ++i) {
		if (_defined[i].name == name && _defined[i][0]._childs.size() == par_count) { // проверяем необходимые условия
			return &_defined[i];
		}
	}
	if (perv != nullptr) {
		return perv->get_func_owerload(name, par_count);
	}
	return nullptr;
}

bool mf::stack_frame::have(std::string _name, obj_env_type type, bool global_search){
	switch (type) {
		case obj_env_type::_obj:
		    return have(_defined, _name, type, global_search); // поиск переменных и функций
		case obj_env_type::_type:
		    return have(types, _name, type, global_search); // поиск типов 
	}
}

bool mf::stack_frame::have(unit_vector & vect, std::string _name, obj_env_type type, bool global_search){
	for (size_t i = 0; i < vect.size(); ++i) {
		if (vect[i].name == _name) {
			return true;
		}
	}
    if(global_search && perv != nullptr){
        return perv->have(_name, type, global_search);
    }
    return false;
}

void mf::stack_frame::add(unit & _unit, obj_env_type type){
	switch (type) {
	case obj_env_type::_obj:
		_defined.push_back(_unit);
		break;
	case obj_env_type::_type:
		types.push_back(_unit);
		break;
	}
}

mf::_type mf::stack_frame::get_type(std::string name){
	for (int count = 0; count < _defined.size(); count++) { // проверка не переменная/функция/ссылка
		if (_defined[count].name == name) {
			return _defined[count].type;
		}
		if (_defined[_defined.size() - count - 1].name == name) {
			return _defined[_defined.size() - count - 1].type;
		}
	}
	for (int count = 0; count < types.size(); count++) { // проверка не тип
		if (types[count].name == name) {
			return types[count].type;
		}
	}
	if (perv != nullptr) {
		return perv->get_type(name); // запрос в предыдущий кадр
	}
	return _type::_indentf; // если нет ничего возвращаем неопределенный тип
}

mf::stack_frame * mf::stack_frame::get_perv(){
	return perv;
}

void mf::stack_frame::delete_var(std::string name){
	for (size_t i = 0; i < _defined.size(); i++) {
		if (_defined[i].name == name) {
			_defined.erase(_defined.begin() + i); // удаляем переменную
			return;
		}
	}
	if (perv != nullptr) {
		perv->delete_var(name);
	}
	throw std::string(name + " -> неизвестный элемент").c_str();
}

void mf::stack_frame::set_flag(uint8_t flag){
	flags |= flag;
}

void mf::stack_frame::reset_flag(uint8_t flag){
	flags ^= flag & (flags & flag);
}

bool mf::stack_frame::check_flag(uint8_t flag){
	return flags & flag;
}

mf::unit & mf::stack_frame::get(unit_vector & vect, std::string _name, obj_env_type type){
	for (int count = 0; count < vect.size(); count++) {
		if (vect[count].name == _name) {
			return vect[count];
		}
		if (vect[vect.size() - count - 1].name == _name) {
			return vect[vect.size() - count - 1];
		}
	}
	if (this->perv != nullptr) {
		return this->perv->get(_name, type);
	}
}
