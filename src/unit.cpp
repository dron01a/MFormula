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

unit & unit::operator[](int position){
    return _childs[position];
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