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