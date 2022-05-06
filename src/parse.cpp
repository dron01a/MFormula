#include "parse.h"

_units parse(_units & _tokens,environment & env){
    _units _result; // result of function
    std::stack<unit> _operations; // stack with operations +-/* ... 
    for (size_t i = 0; i < _tokens.size(); i++){
        if(_key_words_parse.count(_tokens[i].type) != 0){
            _result.push_back(_key_words_parse[_tokens[i].type](_tokens,env,i));
            continue;
        }
        switch (_tokens[i].type){
        case _type::_break:
        case _type::_continue:
        case _type::_bool:
        case _type::_string:
        case _type::_var:
        case _type::_num:
        case _type::_list:
            _result.push_back(_tokens[i]);
            break;
        case _type::_indentf:
            if(env.have(_tokens[i].name)){
                _tokens[i].type = env.get(_tokens[i].name).type;
                i--;  // step back to parse token 
            }
            break;
        default:
            throw error(_tokens[i], "unknown variable or function!");
            break;
        }
    }
    return _result;
}

_units copy_from(_units & _tokens, size_t _begin, size_t _end){
    return {_tokens.begin() + _begin, _tokens.end() - (_tokens.size() - _end)};
}

_units cut_from(_units & _tokens, size_t & _begin, size_t _end){
    _units result = copy_from(_tokens,_begin,_end);
    _begin = _end;
    return result;
}

size_t find_close_brt(_units & _tokens, size_t position){
    int result; // position of close brt 
    std::string type; // type of brt
    int open_chars = 1; // currient count of open brt 
    switch(_tokens[position].name[0]){
        case '(':type = ")";break;     // set type o brt 
        case '[':type = "]";break;   
        case '{':type = "}";break;
    }
    for(int count = position + 1;; count++){
        if(_tokens[count].name == _tokens[position].name){
            open_chars++;
        }
        if(_tokens[count].name == type){
            open_chars--;
        }
        if(open_chars == 0){
            result = count; // save result 
            break;
        }
    }
    if(open_chars != 0){
        throw error(_tokens[position],"not found closing character \"" + type + "\"");
    }
    return result;
}

unit parse_var_init(_units & _tokens,environment & env, size_t & position){
    unit _result(_tokens[position]);
    if(env.have(_tokens[position+1].name)){
        throw error(_tokens[position+1], "var \"" + _tokens[position+1].name + "\" already defined");
    } 
    position++;
    while(_tokens[position].type != _type::_semicolon){
        unit new_var(_type::_var,_tokens[position].name);
        if(_tokens[position+1].name == "="){
            position+=2;
            if(_tokens[position].name == "{"){ // if list
                position++;
                new_var.type = _type::_list;
                new_var._childs = parse_list(cut_from(_tokens,position,find_close_brt(_tokens,position - 1)));
            }
            else{ // if variable
                size_t stop_pos = position; // stop position of cut_from
                while(_tokens[stop_pos].type != _type::_semicolon){
                    if(_tokens[stop_pos].type == _type::_func || _tokens[stop_pos].type == _type::_coreFunc){
                        stop_pos = find_close_brt(_tokens,stop_pos + 1); // set new stop position 
                    }
                    if(_tokens[stop_pos].type == _type::_special && _tokens[stop_pos+1].type == _type::_indentf){
                        stop_pos++;
                        break;
                    }
                    stop_pos++;
                }
                new_var._childs = cut_from(_tokens, position, stop_pos); // set units 
            }
        }
        env.add(new_var); // add var to env
        _result._childs.push_back(unit(new_var.type,new_var.name)); // add var inti to 
        if(position < _tokens.size() - 1){
            switch (_tokens[position + 1].type){
            case _type::_special: position+=2; break;
            case _type::_semicolon: position++; break;
            }
        }
    }
    if(_tokens[position + 2].type == _type::_semicolon ){
        position+=2;
    }
    return _result;
}

_units parse_list(_units _tokens){
    _units result;
    if(_tokens.size() != 0){
        result.push_back(unit());
    }
    for(size_t count = 0; count < _tokens.size(); count++){
        if(_tokens[count].type == _type::_special){
            result.push_back(unit());
            continue;
        }
        if(_tokens[count].name == "{"){
            result[result.size()-1].type = _type::_list;
            result[result.size()-1]._childs = parse_list(copy_from(_tokens,count + 1,find_close_brt(_tokens,count)));
            count = find_close_brt(_tokens,count);
            continue;
        }
        result[result.size()-1]._childs.push_back(_tokens[count]);
    }
    return result; 
}

unit parse_func_init(_units & _tokens,environment & env, size_t & position){
    unit result; 
    position++;
    unit func = parse_token_condition(_tokens,env,position); // get body of function 
    func.type = _type::_func;  // set type 
    func[0]._childs.push_back(unit(_type::_semicolon,";"));
    env.add(func); // add to env 
    result._childs.push_back(func);
    result.type = _type::_functionInit;
    return result; 
}

unit parse_token_condition(_units & _tokens,environment & env, size_t & position){
    unit result(_tokens[position]);
    result._childs.resize(2);
    position+=2;        // cut condition 
    result[0]._childs = cut_from(_tokens,position,find_close_brt(_tokens,position - 1));
    position+=2;       // cut body 
    result[1]._childs = cut_from(_tokens,position,find_close_brt(_tokens,position - 1));
    return result;
}

unit parse_if(_units & _tokens,environment & env, size_t & position){
    unit result = parse_token_condition(_tokens,env,position);
    result.type = _type::_if;
    if(_tokens[position + 1].type == _type::_else){
        result._childs.push_back(unit());
        position+=3;
        result[2]._childs = cut_from(_tokens,position,find_close_brt(_tokens,position - 1));
    }
    return result;
}

unit parse_loop(_units & _tokens,environment & env, size_t & position){
    unit result = parse_token_condition(_tokens,env,position);
    return result;
}
