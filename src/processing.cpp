#include "processing.h"

unit_vector set_vars(unit_stack &args, int _count){
    if(args.size() < _count){
        throw "error _count!";
    }
    unit_vector result;
    result.resize(_count);
    for(int i = _count; i != 0; i--){
        result[i-1] = args.top();
        args.pop();
    }
    return result;
}

unit & value(unit & node){
    if(node.__mem != nullptr){
        return *node.__mem; // return pointer
    }
    return node; // return node
}

void set_in_env(std::stack<unit> & params, environment & env){
    unit_vector _params = set_vars(params,2); // vector with params 
    unit _temp;
    if(_params[0].__mem == nullptr){
        _temp.__mem = & env.get(_params[0].name)._childs[value(_params[1]).to_int()];
    }
    else{
        _temp.__mem = & _params[0].__mem->_childs[value(_params[1]).to_int()];
    }
    params.push(_temp);
}

void var_init(unit & node, environment & env){
    for(size_t _vars = 0;_vars < node._childs.size(); _vars++ ){
        unit_vector childs = env.get(node._childs[_vars].name)._childs;  
        if(env.get(node._childs[_vars].name).type == _type::_list){
            unit listNode;
            listNode = env.get(node._childs[_vars].name);
            eval_list(listNode, env);
            env.get(node._childs[_vars].name) = listNode;
        }
        else{
            if(childs.size() != 0){
                childs = parse(childs, env);
                eval(childs, env); 
                if(childs[0].type == _type::_list){
                    env.get(node._childs[_vars].name).assign(value(childs[0]));
                    env.get(node._childs[_vars].name).type = _type::_list;
                }
                else{
                    env.get(node._childs[_vars].name)._childs = childs;
                }
            }
        }
    }
}

void eval_list(unit & node, environment & env){
    for(size_t count = 0; count < node._childs.size(); count++){
        if(node[count].type == _type::_list){
            eval_list(node[count], env);
            continue;
        }
        node[count]._childs = parse(node[count]._childs, env);
        eval(node[count]._childs, env);
        node[count] = node[count][0];
    }
}
