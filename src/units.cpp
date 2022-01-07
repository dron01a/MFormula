#include "units.h"

unit::unit(_type t,std::string s, int pr){
    type = t;
    name = s;
    prior = pr;
    if(type == _type::_num){
        if (name.find(".",0) != NPOS){
            for(int _i = name.size() -1; _i != name.find(".") - 1; _i--){
                if(name[_i] == '0'){
                    name.pop_back();
                }
                else{
                    break;
                }
            }
        }
        if(name[name.size() -1 ] == '.'){
            name.pop_back();    
        }
    }
}

unit::unit(bool _val){
    if(_val == true){
        name = "true";
    }
    else{
        name = "false";
    }
    type = _type::_bool;
    prior = 0;
}

unit::unit(double _num){
    type = _type::_num;
    name = std::to_string(_num);
    if (name.find(".",0) != NPOS){
        for(int _i = name.size() -1; _i != name.find(".") - 1; _i--){
            if(name[_i] == '0'){
                name.pop_back();
            }
            else{
                break;
            }
        }
    }
    if(name[name.size() -1 ] == '.'){
        name.pop_back();    
    }
    prior = 0;
}

int unit::to_int() const{
    switch (type){
    case _type::_num:
        return std::atoi(name.c_str());
        break;
    case _type::_var:
        return _childs[0].to_int();
    default:
        throw "error type";
        break;
    }
}

bool unit::to_bool() const{
    switch (type){
    case _type::_bool:
        if( name == "true"){
            return true;
        }
        if(name == "false"){
            return false;
        }
        throw "error type";
        break;
    case _type::_var:
        return _childs[0].to_bool();
    default:
        throw "error type";
        break;
    }
}

double unit::to_double() const{
    switch (type){
    case _type::_num:
        return std::stof(name);
        break;
    case _type::_var:
        return _childs[0].to_double();
        break;
    default:
        throw "error type";
        break;
    }
}

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

//void unit::push_back(unit unit){
//    if(type == _type::_list){
//        _childs.push_back(unit);
//    }
//    throw "error type";
//}

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
        printf("%s %s \n","{",_childs[0].name.c_str());
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
        return this->to_double() + _unit.to_double();
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
        return this->to_double() - _unit.to_double();
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
        return this->to_double() * _unit.to_double();
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
        return this->to_double() / _unit.to_double();
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
        return double(this->to_int() % _unit.to_int());
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

bool unit::operator==(const unit & _unit) const{
    switch (type){
    case _type::_num:
        return this->to_double() == _unit.to_double();
        break;
    case _type::_bool:
    case _type::_text:
        return this->to_string() == _unit.to_string();
        break;
    case _type::_var:
        return this->_childs[0] == _unit;
        break;
    case _type::_list: 
        if(_unit.type != _type::_list){
            throw "error of type";
        }
        return this->_childs == _unit._childs;
        break;
    default:
        if(name == _unit.name){
            return true;
        }  
        throw "error of type";
        break;
    }
}

bool unit::operator!=(const unit & _unit) const{
    return !(*this == _unit);
}


bool unit::operator>(unit & _unit) const {
    if(_unit.type != _type::_num && _unit.type != _type::_var){
        throw "type error";
    }
    switch (type){
    case _type::_var:
    case _type::_num:
        return this->to_double() > _unit.to_double();
    default:
        throw "type error";
    }
}

bool unit::operator<(unit & _unit) const {
    if(_unit.type != _type::_num && _unit.type != _type::_var){
        throw "type error";
    }
    switch (type){
    case _type::_var:
    case _type::_num:
        return this->to_double() < _unit.to_double();
    default:
        throw "type error";
    }
}

bool unit::operator>=(unit & _unit) const {
    return !(*this < _unit);
}

bool unit::operator<=(unit & _unit) const {
    return !(*this > _unit);
}

bool unit::operator&&(unit & _unit) const {
    if(type == _type::_bool){
        return this->to_bool() && _unit.to_bool();    
    }
}

bool unit::operator||(unit & _unit) const {
    if(type == _type::_bool){
        return this->to_bool() || _unit.to_bool();    
    }
}

unit unit::increment(){
    if(type == _type::_var ){
        return unit((double)to_int()+1);
    }
    throw "type error";
}

unit unit::decrement(){
    if(type==_type::_var){
        return unit((double)to_int()-1);
    }
    throw "type error";
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

void environment::saveChange(environment & env){
    for(int i = 0; i < env.defined().size(); i++){
        if(have(env.defined()[i].name)){
            get(env.defined()[i].name) = env.defined()[i];
        }
    }
}

void environment::comb(environment & env){
    for(int i = 2; i < env.defined().size(); i++){
        if(!have(env.defined()[i].name)){
            _defined.push_back(env.defined()[i]);
        }
    }
}