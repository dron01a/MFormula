#include "processing.h"

void eval(unit_vector & tokens,environment & env){
    environment _local(env); // local envinronment 
    unit_stack params;       // stack with args 
    for(size_t count = 0; count < tokens.size(); count++){
        if(key_words_iterpr.count(tokens[count].type) != 0){
            key_words_iterpr[tokens[count].type](tokens[count],_local); // interpretation key words
            continue;
        }
        switch (tokens[count].type){
        case _type::_string:
        params.push(tokens[count]);
            break;
        case _type::_var:
        case _type::_list:
            tokens[count].__mem = & _local.get(tokens[count].name);
        case _type::_num:
            params.push(tokens[count]);
            break;
        case _type::_opr:
        case _type::_coreFunc:
            if(mem_func.count(tokens[count].name) != 0){
                mem_func[tokens[count].name](params,_local);
                continue;
            }
            params.push(opr_proc(params,tokens[count].name,tokens[count].prior));
            break;
        case _type::_func:
            call_func(tokens[count],params,_local);
            break;
        case _type::_return:
            params.top() = value(params.top());
            count = tokens.size();
            break;
        case _type::_indentf:
            if(_local.have(tokens[count].name)){
                tokens[count].type = _local.get(tokens[count].name).type;
                count--;
            }
            break;
        case _type::_break:  
        case _type::_continue:
            {
                _ctrl _thr(_local,tokens[count].type); 
                throw _thr;
            }
            break;
        }
    }
    env.save_change(_local);
    tokens.clear();
    if(params.size() != 0){
        tokens.push_back(value(params.top()));
    }
}

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

void set_in_env(unit_stack & params, environment & env){
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

void if_iterpr(unit & node, environment & env){
    environment _local(env);
    unit_vector cond = parse(node._childs[0]._childs,env);
    unit_vector expr;
    eval(cond,env);
    if(cond[0].to_bool() == true){
        expr = node._childs[1]._childs;
    }
    else if(node._childs.size() == 3){
        expr = node._childs[2]._childs;
    }   
    if(expr.size() != 0){
        expr = parse(expr, _local);
        eval(expr,_local);
    }
    env.save_change(_local);
}

void while_interpt(unit & node, environment & env){
    environment _local(env);
    unit_vector cond = parse(node._childs[0]._childs,env); // condition 
    unit_vector expr = parse(node._childs[1]._childs,env); // loop 
    loop(expr, cond, cond, env, _local);
}

void for_interpt(unit & node, environment & env){
    environment _local(env);
    auto del1 = std::find_if(node._childs[0]._childs.begin(), node._childs[0]._childs.end(),[](unit & _unit){
        return _unit.type == _type::_semicolon;
    });
    auto del2 = std::find_if(del1 + 1, node._childs[0]._childs.end(),[](unit & _unit){
        return _unit.type == _type::_semicolon;
    });
    unit_vector init = { node._childs[0]._childs.begin(), del1 + 1 }; // initial conditions
    init = parse(init,_local); 
    unit_vector cond = {del1 + 1, del2 }; // condition of loop 
    cond = parse(cond,_local);
    unit_vector step = { del2 + 1, node._childs[0]._childs.end()}; // step of loop 
    step = parse(step,_local);
    unit_vector expr = parse(node._childs[1]._childs,_local); // loop 
    eval(init,_local);
    loop(expr, cond, step, env, _local);  
}

void loop(unit_vector & _expr, unit_vector & _cond, unit_vector & _step, environment & _global, environment & _local){
    unit_vector cond = _cond;
    unit_vector expr;
    unit_vector step;
    eval(cond,_local);
    while(cond[0].to_bool() == true){
        step = _step;
        cond = _cond;
        try{
            expr = _expr;
            eval(expr,_local);
        }
        catch(_ctrl _T){
            _local.save_change(_T.env);
            _global.save_change(_local);
            if(_T.type == _type::_continue){
                eval(step,_local);
                eval(cond,_local);
                continue;
            }  
            if(_T.type == _type::_break){
                cond[0].name = "false";
                break;
            }
        }
        eval(step,_local);
        eval(cond,_local);
    }
    _global.save_change(_local);
}

void func_init(unit & node, environment & env){
    /*
        comming son
    */   
}

void assign(unit_stack & _params, environment & env){
    unit_vector params = set_vars(_params,2);
    value(params[0]).assign(value(params[1]));
}

void decrem(unit_stack & params, environment & env){
    if(value(params.top()).type == _type::_var ){
        value(params.top())[0] = value(params.top())[0].decrement();
    }
    if(value(params.top()).type == _type::_num ){
        value(params.top()) = value(params.top()).decrement();
    }
    params.pop();
}


void increm(unit_stack & params, environment & env){
    if(value(params.top()).type == _type::_var ){
        value(params.top())[0] = value(params.top())[0].increment();
    }
    if(value(params.top()).type == _type::_num ){
        value(params.top()) = value(params.top()).increment();
    }
    params.pop();
}

double factorial(double n){
    if(n==0){
        return 1;
    }
    return n*factorial(n-1);
}

unit opr_proc(unit_stack & args, std::string exp, int prior){
    if(args.size() == 0){
        throw std::string("operation:" + exp +" --> no arguments!");
    }
    if((prior > 0  && exp != "!" && exp != "nvar") || exp == "log" || exp == "&&" || exp == "||"){
        unit_vector _params = set_vars(args,2);
        return binary_funcs[exp](value(_params[0]),value(_params[1]));
    }
    else{
        unit_vector _params = set_vars(args,1);
        return simple_funcs[exp](value(_params[0]));
    }
}

void call_func(unit & node, unit_stack & params,  environment & env){
    environment _local; 
    unit _func = env.get(node.name);
    unit_vector _par = _func._childs[0]._childs;    // function parametrs 
    unit_vector _body = _func._childs[1]._childs;   // budy of function 
    _par = parse(_par, _local); // parse parametrs of functions 
    unit_vector _func_args = set_vars(params,_par[0]._childs.size());
    _local.comb(env);
    for (int count = 0; count < _func_args.size(); count++ ){
        _local.defined()[count+2].assign(value(_func_args[count]));
    }
    _body = parse(_body,_local); // parse body of function
    eval(_body,_local);
    if(_body.size() != 0){
        params.push(value(_body[0])); // set output 
    }
}