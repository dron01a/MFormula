#include "parse.h"

_units parse(_units & _tokens,environment & env){
    _units _result; // result of function
    std::stack<unit> _operations; // stack with operations +-/* ... 
    for (int i = 0; i < _tokens.size(); i++){
        switch (_tokens[i].type){
        case _type::_varInit:
            _result.push_back(parce_var_init(_tokens,env,i)); // parce _type:: var  
            break;
        case _type::_break:
        case _type::_continue:
        case _type::_bool:
        case _type::_string:
        case _type::_var:
        case _type::_num:
        case _type::_list:
            _result.push_back(_tokens[i]);
            break;
        default:
            break;
        }
    }
    return _result;
}

unit parce_var_init(_units & _tokens,environment & env, int & position){
    int cur_pos = position;
    if(env.have(_tokens[position+1].name)){
        throw "var \"" + _tokens[position+1].name + "\" already defined";
    } 
    position++;
    
}
