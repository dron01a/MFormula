#include "unit.h"

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

unit::unit(long double _num){
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

unit & unit::operator[](int position){
    return _childs[position];
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

long double unit::to_double() const{
    switch (type){
    case _type::_num:
        return std::stold(name);
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
    case _type::_string:
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

int unit::size(){
    switch (type)
    {
    case _type::_var:
    case _type::_list:
        return _childs.size();
    default:
        break;
    }
}

unit unit::increment(){
    if(type == _type::_num ){
        return unit((long double)to_int()+1);
    }
    throw "type error";
}

unit unit::decrement(){
    if(type == _type::_num ){
        return  unit((long double)to_int()-1);
    }
    throw "type error";
}

void unit::resize(int _size){
    switch (type){
    case _type::_var:
    case _type::_list:
        if(_size < 0){
            throw "size < 0";
        }
        _childs.resize(_size);
    default:
        break;
    }
}

void unit::print(){
    switch (type){
    case _type::_num:
    case _type::_string:
        printf("%s",name.c_str());
        break;
    case _type::_var:
        _childs[0].print();
        break;
    case _type::_list:
        printf("{ ");
        _childs[0].print();
        for(int step = 1; step < _childs.size(); step++){
            printf(" , ");
            _childs[step].print();
        }
        printf("%s\n"," }");
        break; 
    default:
        throw "error of type";
        break;
    }
}

void unit::assign(unit _unit){
    switch (_unit.type){
    case _type::_num:
    case _type::_string:
        if(type == _type::_var){
            if(_childs.size() == 0){
                _childs.push_back(unit());
            }
            _childs[0] =  _unit;
        }
        else{
            *this = _unit;
        }
        break;
    case _type::_list: 
    case _type::_var:
        type = _unit.type;
        _childs = _unit._childs;
        break;
    default:
        throw error(_unit,"error of type");
        break;
    }
}

std::istream& operator>> (std::istream& _stream,unit & unit){
    std::getline(_stream,unit.name);
    if(unit.name.find_first_not_of(".0123456789") != NPOS){
        unit.type = _type::_string;
    } 
    else{
        unit.type = _type::_num;    
    }
    return _stream;
}

std::ostream& operator<< (std::ostream& _stream,unit & unit){
    switch (unit.type){
    case _type::_num:
    case _type::_string:
        _stream << unit.name;
        break;
    case _type::_var:
        _stream << unit._childs[0];
        break;
    case _type::_list:
        _stream << "{ " << unit._childs[0];
        for(int step = 1; step < unit._childs.size(); step++){
            _stream << ", " << unit._childs[step];
        }
        _stream << "}\n";
        break; 
    default:
        throw error(unit,"error of type");
        break;
    }
    return _stream;
}

unit unit::operator+(unit & _unit) const{
    switch (type){
    case _type::_num:
        return this->to_double() + _unit.to_double();
        break;
    case _type::_string:
        if(_unit.type == _type::_var){
            if(_unit._childs[0].type == _type::_string){
                return unit(this->type,name + _unit._childs[0].name);
            }
        }
        if(_unit.type == _type::_string){
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
    case _type::_string:
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
    case _type::_string:
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
    case _type::_string:
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
        return (long double)(this->to_int() % _unit.to_int());
        break;
    case _type::_string:
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
    case _type::_string:
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

void unit::operator+=(unit & _unit){
    switch (type){
    case _type::_var:
    case _type::_string:
        this->assign(*this + _unit);
        break;
    default:
        throw error(_unit,"error of type");
        break;
    }
}

void unit::operator-=(unit & _unit){
    switch (type){
    case _type::_var:
    case _type::_string:
        this->assign(*this - _unit);
        break;
    default:
        throw error(_unit,"error of type");
        break;
    }
}

void unit::operator*=(unit & _unit){
    switch (type){
    case _type::_var:
    case _type::_string:
        this->assign(*this * _unit);
        break;
    default:
        throw error(_unit,"error of type");
        break;
    }
}

void unit::operator/=(unit & _unit){
    switch (type){
    case _type::_var:
    case _type::_string:
        this->assign(*this / _unit);
        break;
    default:
        throw error(_unit,"error of type");
        break;
    }
}

void unit::operator%=(unit & _unit){
    switch (type){
    case _type::_var:
    case _type::_string:
        this->assign(*this % _unit);
        break;
    default:
        throw error(_unit,"error of type");
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

unit_vector & environment::defined(){
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

void environment::save_change(environment & env){
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

std::ostream& operator<< (std::ostream& _stream,error & _error){
    _stream << "line:" << _error._unit._str; 
    _stream << " col:" << _error._unit._col;
    _stream << "\nerror:" << _error.message << "\n\n";
    return _stream;
}