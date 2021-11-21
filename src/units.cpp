#include "units.h"

unit::unit(_type t,std::string s){
    type = t;
    name = s;
}

unit::unit(_type t,std::string s, int pr){
    type = t;
    name = s;
    prior = pr;
}

unit::unit(bool _val){
    if(_val == true){
        name = true;
    }
    else{
        name = false;
    }
    type = _type::_bool;
    prior = 0;
}

unit::unit(double _num){
    type = _type::_num;
    name = std::to_string(_num);
    prior = 0;
}

unit::operator int() const{
    switch (type){
    case _type::_num:
        return std::atoi(name.c_str());
        break;
    case _type::_var:
        return _childs[0];
    default:
        throw "error type";
        break;
    }
}

unit::operator double() const{
    switch (type){
    case _type::_num:
        if (name.find('.',0) != NPOS){
            for(int _i = name.size() -1; _i != name.find("."); _i--){
                if(name[_i] != '0'){
                    return std::strtod(name.substr(0,_i + 1).c_str(),nullptr);
                }
            }
        }
        return std::strtod(name.c_str(),nullptr);
        break;
    case _type::_var:
        return _childs[0];
        break;
    default:
        throw "error type";
        break;
    }
}

void unit::print(){
    switch (type){
    case _type::_num:
    case _type::_text:
        printf("%s\n",name.c_str());
        break;
    case _type::_var:
        _childs[0].print();
        break;
    case _type::_list:
        printf("%c %s \n","{",_childs[0].name.c_str());
        for(int step = 1; step < _childs.size(); step++){
            printf(", %s\n",_childs[step]);
        }
        printf("%s\n","}");
        break; 
    default:
        throw "error of type";
        break;
    }
}

void unit::assign(unit _unit){
    switch (_unit.type){
    case _type::_num:
    case _type::_text:
        if(_childs.size() != 0 ){
            _childs[0] = _unit;
        }
        else{
            _childs.push_back(_unit);
        }
        break;
    case _type::_list: 
    case _type::_var:
        _childs = _unit._childs;
        break;
    default:
        throw "error of type";
        break;
    }
}

unit unit::operator+(unit & _unit) const{
    switch (type){
    case _type::_num:
        return(double(*this) + double(_unit));
        break;
    case _type::_text:
        if(_unit.type == _type::_var){
            if(_unit._childs[0].type == _type::_text){
                return unit(this->type,name + _unit._childs[0].name);
            }
        }
        if(_unit.type == _type::_text){
            return unit(this->type,name + _unit.name);
        }
        throw "error type";
        break;
    case _type::_var:
        return unit(_childs[0] + _unit);
        break;
    case _type::_list: 
        break;
    default:
        throw "error of type";
        break;
    }
}

unit unit::operator-(unit & _unit) const{
    switch (type){
    case _type::_num:
        return(double(*this) - double(_unit));
        break;
    case _type::_text:
        throw "error type";
        break;
    case _type::_var:
        return unit(_childs[0] - _unit);
        break;
    case _type::_list: 
        break;
    default:
        throw "error of type";
        break;
    }
}

unit unit::operator*(unit & _unit) const{
    switch (type){
    case _type::_num:
        return double(*this) * double(_unit);
    case _type::_text:
        throw "error type";
        break;
    case _type::_var:
        return unit(_childs[0] * _unit);
        break;
    case _type::_list: 
        break;
    default:
        throw "error of type";
        break;
    }
}

unit unit::operator/(unit & _unit) const{
    switch (type){
    case _type::_num:
        return(double(*this) / double(_unit));
        break;
    case _type::_text:
        throw "error type";
        break;
    case _type::_var:
        return unit(_childs[0] / _unit);
        break;
    case _type::_list: 
        break;
    default:
        throw "error of type";
        break;
    }
}

unit unit::operator%(unit & _unit) const{
    switch (type){
    case _type::_num:
        return double((int)*this % (int)_unit);
        break;
    case _type::_text:
        throw "error type";
        break;
    case _type::_var:
        return unit(_childs[0] % _unit);
        break;
    case _type::_list: 
        break;
    default:
        throw "error of type";
        break;
    }
}

//bool unit::operator==(unit & _unit) const {
//    switch (type){
//    case _type::_num:
//        return double(*this) == double(_unit);
//        break;
//    case _type::_bool:
//    case _type::_text:
//        return this->to_string() == _unit.to_string();
//        break;
//    case _type::_var:
////        return *this == _unit;
//        break;
//    case _type::_list: 
//        if(_unit.type != _type::_list){
//            throw "error of type";
//        }
//        return _childs == _unit._childs;
//        break;
//    default:
//        throw "error of type";
//        break;
//    }
//}

//bool unit::operator!=(unit & _unit) const {
//    return !(*this == _unit);
//}

std::string unit::to_string() const{
    switch (type){
    case _type::_num:
    case _type::_text:
    case _type::_bool:
        return name;
        break;
    case _type::_var:
        return _childs[0].name;
        break;
    default:
        break;
    }
}

environment::environment(){
    _defined[0]._childs.push_back(unit(_type::_num,"3.1415926535"));
    _defined[1]._childs.push_back(unit(_type::_num,"2.7182818284"));
}

environment::environment(environment &env){
    this->_defined = env.defined(); 
}

_units & environment::defined(){
    return _defined;
}

void environment::add(unit & _unit){
    _defined.push_back(_unit);
}

unit & environment::get(std::string _name){
    for(int count = 0; count < _defined.size(); count++){
        if(_defined[count].name == _name){
            return _defined[count];
        }
    }
    throw "undef element";
}

bool environment::have(std::string _name){
    for (unit iter : _defined){
        if(iter.name == _name){
            return true;
        }
    }
    return false;
}