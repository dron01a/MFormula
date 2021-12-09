#include "interpretation.h"

void run(std::string _script){
    _units u;
    environment env;
    Lexer lex(_script, env);
    u = lex.getUnits();
    Parcer par(u,env);
    u = par.getTokens();
    eval(u,env);
}

void varInit(unit & node, environment & env){
    _units childs = env.get(node._childs[0].name)._childs;  
    if(env.get(node._childs[0].name).type == _type::_list){
        for(int count = 0; count < childs.size(); count++){
            Parcer childsParcer(childs[count]._childs,env);
            childs[count]._childs = childsParcer.getTokens();
        } 
        for(int count = 0; count < childs.size(); count++){
            eval(childs[count]._childs, env);
            env.get(node._childs[0].name)._childs[count] = childs[count]._childs[0];
        }
    }
    else{
        Parcer childsParcer(childs,env);
        childs = childsParcer.getTokens();
        eval(childs, env); 
        env.get(node._childs[0].name)._childs = childs;
    }
}

void eval(_units & tokens,environment &env){
    std::stack<unit> params; 
    for(int count = 0; count < tokens.size(); count++){
        switch (tokens[count].type){
        case _type::_varInit:
            varInit(tokens[count],env);
            break;  
        case _type::_if:
            if_iterpr(tokens[count],env);
            break;
        case _type::_num:
        case _type::_text:
            params.push(tokens[count]);
            break;
        case _type::_var:
            params.push(env.get(tokens[count].name));
            break;
        case _type::_list:{
                if(params.size() != 0){
                    if(params.top().type == _type::_num || params.top().type == _type::_var){
                        unit listVal(_type::_var,"listVar");
                        listVal._childs.push_back(env.get(tokens[count].name)._childs[params.top().to_int()]);
                        listVal._childs.push_back(tokens[count]);
                        listVal._childs.push_back(params.top());
                        params.pop();
                        params.push(listVal);
                    }
                    else{
                        params.push(env.get(tokens[count].name));
                    }
                }
                else{
                    params.push(env.get(tokens[count].name));
                }
            }            
            break;
        case _type::_opr:
           if(tokens[count].name == "="){
                _units _params = setVars(params,2);
                if(_params[0].name == "listVar"){
                    env.get(_params[0]._childs[1].name)._childs[_params[0]._childs[2].to_int()] = _params[1];
                }
                else{
                    _params[0].assign(_params[1]);
                    env.get(_params[0].name) = _params[0];
                }
                continue;
           }
        case _type::_coreFunc:
            if(tokens[count].name == "print"){
                    params.top().print();
                    params.pop(); 
                continue;
            }
            params.push(calcUnits(params,tokens[count].name,tokens[count].prior));
            break;
        }
    }
    tokens.clear();
    if(params.size() != 0){
        tokens.push_back(params.top());
    }
}

_units setVars(std::stack<unit> &args, int _count){
    _units result;
    result.resize(_count);
    for(int i = _count; i != 0; i--  ){
        result[i-1] = args.top();
        args.pop();
    }
    return result;
}

unit calcUnits(std::stack<unit> &args, std::string exp, int prior){
    if(args.size() == 0){
        throw std::string("operation:" + exp +" --> no arguments!");
    }
    if((prior > 0 && exp != "!") || exp == "log"){
        _units _params;
        if(args.size() == 1){
            _params = setVars(args,1);
            return simpleFuncs[exp](_params[0]);
        }
        else{
            _params = setVars(args,2);
        }
        return binaryFuncs[exp](_params[0],_params[1]);
    }
    else{
        _units _params = setVars(args,1);
        return simpleFuncs[exp](_params[0]);//simpleFunc(exp,a);
    }
}

double factorial(double n){
    if(n==0){
        return 1;
    }
    return n*factorial(n-1);
}

void if_iterpr(unit & node, environment & env){
    environment _local(env);
    _units cond = node._childs[0]._childs;
    _units expr;
    Parcer _condParce(cond,env);
    cond = _condParce.getTokens();
    eval(cond,env);
    if(cond[0].to_bool() == true){
        expr = node._childs[1]._childs;
    }
    else if(node._childs.size() == 3){
        expr = node._childs[2]._childs;
    }
    if(expr.size() != 0){
        Parcer _exprParce(expr,_local);
        expr = _exprParce.getTokens();
        eval(expr,_local);
    }
    env.saveChange(_local);
}
