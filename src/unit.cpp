#include "unit.h"

mf::unit::unit(_type type, std::string s, int pos){
    this->type = type;
    this->stream_pos = pos;
    switch (this->type){
    case  _type::_num:
        this->double_val = std::stod(s);
        break;
    case _type::_bool:
        if(s == "true"){
            this->bool_val = true;
        }
        else if(s == "false"){
            this->bool_val = false;
        }
        else {
			throw error(this, "Неверный тип");
        }
        break;
    default:
        name = std::move(s);
        break;
    }
}

mf::unit::unit(_type t){
	type = t;
}

mf::unit::unit(bool _val){
    bool_val = _val;
    type = _type::_bool;
    prior = 0;
}

mf::unit::~unit(){

}

mf::unit::unit(long double _num){
    type = _type::_num;
    this->double_val = _num;
}

mf::unit & mf::unit::operator[](int position){
    return _childs[position];
}

int mf::unit::to_int() const{
	if (this->type != _type::_num && this->type != _type::_var) {
		throw error(this, "Неверный тип");
	}
	return (int)value((unit *)this).double_val;
}

bool mf::unit::to_bool() const{
	if (this->type != _type::_bool && this->type != _type::_var) {
		throw error(this, "Неверный тип");
	}
	return value((unit *)this).bool_val;
}

long double mf::unit::to_double() const{
	if (this->type != _type::_num && this->type != _type::_var) {
		throw error(this, "Неверный тип");
	}
	return value((unit *)this).double_val;
}

std::string mf::unit::to_string() const{
    switch (type){
    case _type::_num:
        return std::to_string(double_val);
    case _type::_string:
        return name;
    case _type::_bool:
        return std::to_string(bool_val);
    case _type::_var:
        return _childs[0].to_string();
    default:
        throw error(this, "Неверный тип");
    }
}

int mf::unit::size(){
	return _childs.size();
    switch (type){
	case _type::_string:
		return this->name.size();
    case _type::_var:
		return _childs[0].size();
    case _type::_list:
        return _childs.size();
    default:
		throw error(this, "Неверный тип");
    }
}

mf::unit mf::unit::increment(){
    if(type == _type::_num || type == _type::_var){
        return unit((long double)to_int()+1);
    }
    throw error(*this, "Неверный тип");
}

mf::unit mf::unit::decrement(){
    if(type == _type::_num || type == _type::_var){
        return  unit((long double)to_int()-1);
    }
    throw error(*this, "Неверный тип");
}

void mf::unit::resize(int _size){
	if (_size < 0) {
		throw error(*this, "Новый размер должен быть больше или равен 0");
	}
	if (this->type != _type::_list && this->type != _type::_var) {
		throw error(this, "Неверный тип");
	}
	_childs.resize(_size);
}

void mf::unit::assign(unit & _unit) {
	switch (value(_unit).type) {
	case _type::_num:
	case _type::_bool:
	case _type::_string:
		if (type == _type::_var) {
			if (_childs.size() == 0) {
				_childs.push_back(unit());
			}
			_childs = { value(_unit) };
		}
		else {
			*this = _unit;
		}
		break;
	case _type::_list:
	case _type::_var:
	case _type::_type_var:
		type = _unit.type;
		_childs = value(_unit)._childs;
		break;
	default:
		throw error(_unit, "Неверный тип");
	}
}

std::istream& mf::operator>> (std::istream& _stream, mf::unit & unit){
    std::string temp; // временная переменная для хранения данных
    std::getline(_stream, temp);
    if(unit.name.find_first_not_of(".0123456789") != NPOS){
        unit.type = _type::_string;
        unit.name = temp;
    } 
    else{
        unit.type = _type::_num;
        unit.double_val = stold(temp);    
    }
    return _stream;
}

std::ostream& mf::operator<< (std::ostream& _stream, mf::unit & unit){
    switch (unit.type){
    case _type::_num:
        _stream << unit.double_val;
        break;
    case _type::_string:
        _stream << unit.name;
        break;
    case _type::_bool:
        _stream << unit.bool_val;
        break;
    case _type::_var:
	case _type::_const:
        _stream << unit._childs[0];
        break;
    case _type::_list:
        _stream << "{ " << unit._childs[0];
        for(int step = 1; step < unit._childs.size(); step++){
            _stream << ", " << unit._childs[step];
        }
        _stream << " }\n";
        break; 
    default:
        throw error(unit, "Неверный тип");
        break;
    }
    return _stream;
}

mf::unit mf::unit::operator+(unit & _unit) const{
    switch (type){
    case _type::_num:
        return this->to_double() + _unit.to_double();
    case _type::_string:
		return unit(this->type, name + _unit.to_string());
    case _type::_var:
        return unit(_childs[0] + _unit);
    case _type::_list:
		if (_unit.type == _type::_list) {
			unit temp(_type::_list, "");
			temp._childs = this->_childs;
			temp._childs.insert(std::end(temp._childs), std::begin(_unit._childs), std::end(_unit._childs));
			return temp;
		}
		else {
			unit temp(_type::_list, "");
			temp._childs = this->_childs;
			temp._childs.push_back(value(_unit));
			return temp;
		}
        break;
    default:
        throw error(_unit, "Неверный тип");
    }
}

mf::unit mf::unit::operator-(unit & _unit) const{
    switch (type){
    case _type::_num:
        return this->to_double() - _unit.to_double();
    case _type::_var:
        return unit(_childs[0] - _unit);
    default:
        throw error(_unit, "Неверный тип");
    }
}

mf::unit mf::unit::operator*(unit & _unit) const{
    switch (type){
    case _type::_num:
        return this->to_double() * _unit.to_double();
    case _type::_string:
        if(_unit.type == _type::_num){
            std::string res = ""; 
            for(size_t i = 0; i < _unit.to_int(); i++){
                res += this->to_string();
            }
            return unit(_type::_string, res);
        }
        throw error(_unit, "Неверный тип");
    case _type::_var:
        return unit(_childs[0] * _unit);
    default:
        throw error(_unit, "Неверный тип");
    }
}

mf::unit mf::unit::operator/(unit & _unit) const{
    switch (type){
    case _type::_num:
        return this->to_double() / _unit.to_double();
    case _type::_var:
        return unit(_childs[0] / _unit);
    default:
        throw error(_unit, "Неверный тип");
    }
}

mf::unit mf::unit::operator%(unit & _unit) const{
    switch (type){
    case _type::_num:
        return (long double)(this->to_int() % _unit.to_int());
    case _type::_var:
        return unit(_childs[0] % _unit);
    default:
        throw error(_unit, "Неверный тип");
    }
}

mf::unit mf::unit::operator^(unit & _unit) const {
    switch (type){
    case _type::_num:
        return pow(this->to_double(), _unit.to_double());
    case _type::_var:
        return unit(_childs[0] ^ _unit);
    default:
        throw error(_unit, "Неверный тип");
    }
}


bool mf::unit::operator==(const unit & _unit) const{
    switch (type){
    case _type::_num:
        return this->to_double() == _unit.to_double();
    case _type::_bool:
    case _type::_string:
        return this->to_string() == _unit.to_string();
    case _type::_var:
        return  _unit == this->_childs[0];
    case _type::_list:
        if(_unit.type != _type::_list){
           throw error(_unit, "Неверный тип");
        }
        return this->_childs == _unit._childs;
    default:
        if(name == _unit.name){
            return true;
        }  
        throw error(_unit, "Неверный тип");
    }
}

bool mf::unit::operator!=(const unit & _unit) const{
    return !(*this == _unit);
}

bool mf::unit::operator>(unit & _unit) const {
    switch (type){
    case _type::_var:
    case _type::_num:
        return this->to_double() > _unit.to_double();
    default:
        throw error(_unit, "Неверный тип");
    }
}

bool mf::unit::operator<(unit & _unit) const {
    switch (type){
    case _type::_var:
    case _type::_num:
        return this->to_double() < _unit.to_double();
    default:
        throw error(_unit, "Неверный тип");
    }
}

bool mf::unit::operator>=(unit & _unit) const {
    return !(*this < _unit);
}

bool mf::unit::operator<=(unit & _unit) const {
    return !(*this > _unit);
}

bool mf::unit::operator&&(unit & _unit) const {
    if(type == _type::_bool){
        return this->to_bool() && _unit.to_bool();    
    }
}

bool mf::unit::operator||(unit & _unit) const {
    if(type == _type::_bool){
        return this->to_bool() || _unit.to_bool();    
    }
}

void mf::unit::operator+=(unit & _unit) {
	switch (type) {
	case _type::_list:
		this->_childs.push_back(_unit);
		break;
	case _type::_var:
	case _type::_string:
		this->_childs = { *this + _unit };
		break;
	case _type::_const:
		throw error(this, "Для констант не доступна операция +=");
	default:
		throw error(_unit, "Неверный тип");
	}
}

void mf::unit::operator-=(unit & _unit) {
	switch (type) {
	case _type::_var:
		this->_childs = { *this - _unit };
		break;
	case _type::_const:
		throw error(this, "Для констант не доступна операция -=");
	default:
		throw error(_unit, "Неверный тип");
	}
}

void mf::unit::operator*=(unit & _unit) {
	switch (type) {
	case _type::_var:
		this->_childs = { *this * _unit };
		break;
	case _type::_const:
		throw error(this, "Для констант не доступна операция *=");
	default:
		throw error(_unit, "Неверный тип");
	}
}

void mf::unit::operator/=(unit & _unit) {
	switch (type) {
	case _type::_var:
		this->_childs = { *this / _unit };
		break;
	case _type::_const:
		throw error(this, "Для констант не доступна операция /=");
	default:
		throw error(_unit, "Неверный тип");
	}
}

void mf::unit::operator%=(unit & _unit) {
	switch (type) {
	case _type::_var:
		this->_childs = { *this % _unit };
		break;
	case _type::_const:
		throw error(this, "Для констант не доступна операция %=");
	default:
		throw error(_unit, "Неверный тип");
	}
}

void mf::unit::operator^=(unit & _unit) {
	switch (type) {
	case _type::_var:
		this->_childs = { *this ^ _unit };
		break;
	case _type::_const:
		throw error(this, "Для констант не доступна операция ^=");
	default:
		throw error(_unit, "Неверный тип");
	}
}

std::ostream& mf::operator<< (std::ostream& _stream, error & _error){
    _stream << "строка:" << _error._unit._str; 
    _stream << " столбец:" << _error._unit._col;
    _stream << "\nтекст ошибки:" << _error.message << "\n\n";
    return _stream;
}

mf::unit & mf::original(unit & node){
    if(node.__mem != nullptr){
        return original(*node.__mem); // везвращаем указатель
    }
    return node; // возвращаем саму частицу кода
}

mf::unit mf::value(unit & node){
	unit result;
	switch (node.type){ // в зависимости от типа узла выбираем способ получить переменную
	case _type::_var:
		if (node._childs.size() == 0) {
			node._childs.push_back(unit());
		}
		result = original(node)._childs[0];
		break;
	case _type::_list:
		result.type = _type::_list; 
		result._childs = original(node)._childs;
		break;
	default:// если это и не лист, и не переменная то возвращаем оригинал узла
		result = original(node);
		break;
	}
	return result;
}

mf::unit mf::value(unit * node){
	unit result;
	switch (node->type) { // в зависимости от типа узла выбираем способ получить переменную
	case _type::_var:
		if (node->_childs.size() == 0) {
			node->_childs.push_back(unit());
		}
		result = original(*node)._childs[0];
		break;
	case _type::_list:
		result.type = _type::_list;
		result._childs = original(*node)._childs;
		break;
	default:// если это и не лист, и не переменная то возвращаем оригинал узла
		result = original(*node);
		break;
	}
	return result;
}
