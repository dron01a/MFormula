#include "parse.h"

unit_vector parse(unit_vector & _tokens,environment & env){
    unit_vector _result; // result of function
    unit_stack _operations; // stack with operations +-/* ... 
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
        case _type::_openBrt:
            find_close_brt(_tokens,i);
            if(_tokens[i].name == "{" ){
                unit _temp_list(_type::_list, ""); // temp space of list var 
                _temp_list._childs = parse_list(cut_from(_tokens,i,find_close_brt(_tokens,i - 1)));
                _operations.push(_temp_list);
                continue;
            } 
            _operations.push(_tokens[i]);
            break;        
        case _type::_closeBrt:
            parse_close_brt(_result,_operations,_tokens[i]);
            if(_tokens[i].name == "]"){
                _result.push_back(unit(_type::_opr,"[]"));
            }
            break;
        case _type::_return:
        case _type::_opr:
            push_token_if(_result,_operations,_tokens[i],[](unit _unit, std::stack<unit> & oprs){
                return  ((oprs.top().type == _type::_opr && _unit.prior <= oprs.top().prior) 
                    || oprs.top().type == _type::_func 
                    || oprs.top().type == _type::_coreFunc 
                    || oprs.top().type == _type::_list
                );
            });
            _operations.push(_tokens[i]);
            break;
        case _type::_coreFunc:
        case _type::_func:
            _operations.push(_tokens[i]);
            break;
        case _type::_special:
            push_token_if(_result,_operations,_tokens.back(),[](unit _unit, std::stack<unit> & oprs){ 
                return oprs.top().type != _type::_openBrt; 
            });
            break;
        case _type::_semicolon:
            push_stack_to_output(_result,_operations);
            break;
        case _type::_indentf:
            if(!env.have(_tokens[i].name)){
                throw error(_tokens[i], "unknown variable or function!");
            }
            _tokens[i].type = env.get(_tokens[i].name).type;
            i--;  // step back to parse token 
        break;
        }
    }
    push_stack_to_output(_result,_operations);
    return _result;
}

unit_vector copy_from(unit_vector & _tokens, size_t _begin, size_t _end){
    return {_tokens.begin() + _begin, _tokens.end() - (_tokens.size() - _end)};
}

unit_vector cut_from(unit_vector & _tokens, size_t & _begin, size_t _end){
    unit_vector result = copy_from(_tokens,_begin,_end);
    _begin = _end;
    return result;
}

size_t find_close_brt(unit_vector & _tokens, size_t position){
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

unit parse_var_init(unit_vector & _tokens,environment & env, size_t & position){
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

unit_vector parse_list(unit_vector _tokens){
    unit_vector result;
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

unit parse_func_init(unit_vector & _tokens,environment & env, size_t & position){
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

unit parse_token_condition(unit_vector & _tokens,environment & env, size_t & position){
    unit result(_tokens[position]);
    result._childs.resize(2);
    position+=2;        // cut condition 
    result[0]._childs = cut_from(_tokens,position,find_close_brt(_tokens,position - 1));
    position+=2;       // cut body 
    result[1]._childs = cut_from(_tokens,position,find_close_brt(_tokens,position - 1));
    return result;
}

unit parse_if(unit_vector & _tokens,environment & env, size_t & position){
    unit result = parse_token_condition(_tokens,env,position);
    result.type = _type::_if;
    if(_tokens[position + 1].type == _type::_else){
        result._childs.push_back(unit());
        position+=3;
        result[2]._childs = cut_from(_tokens,position,find_close_brt(_tokens,position - 1));
    }
    return result;
}

unit parse_loop(unit_vector & _tokens,environment & env, size_t & position){
    unit result = parse_token_condition(_tokens,env,position);
    return result;
}

void push_token_if(unit_vector & _tokens, unit_stack & opr, unit token, cond_func func){
    while(opr.size() && func(token, opr)){
        _tokens.push_back(opr.top());
        opr.pop();
    }
}

void push_stack_to_output(unit_vector & _tokens, unit_stack & opr){
    while(opr.size()){
        _tokens.push_back(opr.top());
        opr.pop();
    }
    //push_token_if(_tokens,opr,_tokens.back(),[](unit _unit, unit_stack & oprs){ 
    //    return oprs.size() != 0; 
    //});
}

void parse_close_brt(unit_vector & _tokens,unit_stack & opr, unit token){
    switch (token.name[0]){
    case ')':
        push_token_if(_tokens,opr,token,[](unit _unit, unit_stack & opr){ 
            return opr.top().name != "("; 
        });
        break;
    case ']':
        push_token_if(_tokens,opr,token,[](unit _unit, unit_stack & opr){ 
            return opr.top().name != "["; 
        });
        break;
    case '}':
        push_token_if(_tokens,opr,token,[](unit _unit, unit_stack & opr){ 
            return opr.top().name != "{"; 
        });
        break;
    default:
        error(token, "error type");
        break;
    }
    opr.pop();
}