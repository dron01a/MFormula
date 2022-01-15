#include "interpretation.h"

void run(std::string _script){
    environment env;
    _units codeTokens = parseScript(_script, env);
    eval(codeTokens,env);
}

_units parseScript(std::string _script, environment & env){
    _units result; 
    Lexer lex(_script, env);
    result = lex.getUnits();
    Parser par(result,env);
    result = par.getTokens();
    return result;
}

void varInit(unit & node, environment & env){
    _units childs = env.get(node._childs[0].name)._childs;  
    if(env.get(node._childs[0].name).type == _type::_list){
        for(int count = 0; count < childs.size(); count++){
            Parser childsParcer(childs[count]._childs,env);
            childs[count]._childs = childsParcer.getTokens();
        } 
        for(int count = 0; count < childs.size(); count++){
            eval(childs[count]._childs, env);
            env.get(node._childs[0].name)._childs[count] = childs[count]._childs[0];
        }
    }
    else{
        if(childs.size() != 0){
            Parser childsParcer(childs,env);
            childs = eval(childsParcer, env); 
        }
        env.get(node._childs[0].name)._childs = childs;
    }
}

void funcInit(unit & node, environment & env){
    /*
        comming son
    */   
}

void eval(_units & tokens,environment &env){
    environment _local(env);
    std::stack<unit> params; 
    for(int count = 0; count < tokens.size(); count++){
        switch (tokens[count].type){
        case _type::_varInit:
            varInit(tokens[count],_local);
            break;  
        case _type::_functionInit:
            funcInit(tokens[count],_local);
            break;
        case _type::_if:
            if_iterpr(tokens[count],_local);
            break;
        case _type::_for:
            forInterpt(tokens[count],_local);
            break;
        case _type::_while:
            whileInterpt(tokens[count],_local);
            break;
        case _type::_string:
               evalStrung(tokens[count],_local);
        case _type::_num:
            params.push(tokens[count]);
            break;
        case _type::_var:
            params.push(_local.get(tokens[count].name));
            break;
        case _type::_list:{
                if(params.size() != 0){
                    if(params.top().type == _type::_num || params.top().type == _type::_var){
                        unit listVal(_type::_var,"listVar");
                        listVal._childs.push_back(_local.get(tokens[count].name)._childs[params.top().to_int()]);
                        listVal._childs.push_back(tokens[count]);
                        listVal._childs.push_back(params.top());
                        params.pop();
                        params.push(listVal);
                    }
                    else{
                        params.push(_local.get(tokens[count].name));
                    }
                }
                else{
                    params.push(_local.get(tokens[count].name));
                }
            }            
            break;
        case _type::_opr:
            if(tokens[count].name == "--" || tokens[count].name == "++"){
                _local.get(params.top().name)._childs[0] = simpleFuncs[tokens[count].name](params.top());
                params.pop(); 
                continue;
            }
            if(tokens[count].name == "="){
                assign(params,_local);
                continue;
            }
        case _type::_coreFunc:
            if(tokens[count].name == "print"){
                _units _params = setVars(params,params.size());
                for(size_t _par = 0; _par < _params.size(); _par++){
                    _params[_par].print();
                }
                printf("\n");
                continue;
            }
            params.push(calcUnits(params,tokens[count].name,tokens[count].prior));
            break;
        case _type::_func:
            callFunc(tokens[count],params,_local);
            break;
        case _type::_break:  
            {
                _ctrlConst _thr(_local,_type::_break); 
                throw _thr;
            }
        case _type::_continue:
            {
                _ctrlConst _thr(_local,_type::_continue); 
                throw _thr;
            }
        case _type::_return:
            count = tokens.size();
            break;
        }
    }
    env.saveChange(_local);
    tokens.clear();
    if(params.size() != 0){
        tokens.push_back(params.top());
    }
}

_units eval(Parser & _par, environment & env){
    _units _tokens = _par.getTokens();
    eval(_tokens,env);
    return _tokens;
}

_units setVars(std::stack<unit> &args, int _count){
    _units result;
    result.resize(_count);
    for(int i = _count; i != 0; i--){
        result[i-1] = args.top();
        args.pop();
    }
    return result;
}

unit calcUnits(std::stack<unit> &args, std::string exp, int prior){
    if(args.size() == 0){
        throw std::string("operation:" + exp +" --> no arguments!");
    }
    if((prior > 0 && exp != "++" && exp != "--" && exp != "!" && exp != "nvar") || exp == "log" || exp == "&&" || exp == "||"){
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
    Parser _condParce(node._childs[0]._childs,env);
    _units cond= eval(_condParce,env);
    _units expr;
    if(cond[0].to_bool() == true){
        expr = node._childs[1]._childs;
    }
    else if(node._childs.size() == 3){
        expr = node._childs[2]._childs;
    }
    if(expr.size() != 0){
        Parser _exprParse(expr,_local);
        eval(_exprParse,_local);
    }
    env.saveChange(_local);
}


void whileInterpt(unit & node, environment & env){
    environment _local(env);
    Parser _condParce(node._childs[0]._childs,env);
    Parser _exprParce(node._childs[1]._childs,_local);
    _units cond = eval(_condParce,env);
    _units expr;
    while(cond[0].to_bool() == true){
        expr = _exprParce.getTokens();
        try{
            eval(expr,_local);
        }
        catch(_ctrlConst _T){
            _local.saveChange(_T.env);
            env.saveChange(_local);
            if(_T.type == _type::_continue){
                cond = eval(_condParce,_local);
                continue;
            }  
            if(_T.type == _type::_break){
                cond[0].name = "false";
                break;
            }
        }
        cond = eval(_condParce,_local);
    }
    env.saveChange(_local);
}

void forInterpt(unit & node, environment & env){
    environment _local(env);
    Parser _initParce(node._childs[0]._childs,_local);
    Parser _condParce(node._childs[1]._childs,_local);
    Parser _stepParce(node._childs[2]._childs,_local);
    Parser _exprParce(node._childs[3]._childs,_local);
    _units init = eval(_initParce,_local);
    _units cond = eval(_condParce,_local);
    _units step;
    _units expr;
    while(cond[0].to_bool() == true){
        expr = _exprParce.getTokens();
        try{
            eval(expr,_local);
        }
        catch(_ctrlConst _T){
            _local.saveChange(_T.env);
            env.saveChange(_local);
            if(_T.type == _type::_continue){
                step = eval(_stepParce,_local);
                cond = eval(_condParce,_local);
                continue;
            }  
            if(_T.type == _type::_break){
                cond[0].name = "false";
                break;
            }
        }
        step = eval(_stepParce,_local);
        cond = eval(_condParce,_local);
    }
    env.saveChange(_local);
}

void callFunc(unit & node, std::stack<unit> & _prms, environment & env){
    environment _local; 
    unit _func = env.get(node.name);
    _units _params = _func._childs[0]._childs;
    _units _expr = _func._childs[1]._childs;
    Parser _parsParm(_params, _local);
    _params = _parsParm.getTokens();
    _units _functionPar = setVars(_prms, _params.size());
    _local.comb(env);
    for(int _parC = 0; _parC < _params.size(); _parC++){
        _local.defined()[_parC+2].assign(_functionPar[_parC]);
    }
    Parser _parsExpr(_expr, _local);
    _expr = eval(_parsExpr, _local);  
    if(_expr.size() != 0){
        _prms.push(_expr[0]);
    }
}

void assign(std::stack<unit> & params, environment & env){
    _units _params = setVars(params,2);
    if(_params[0].name == "listVar"){
        env.get(_params[0]._childs[1].name)._childs[_params[0]._childs[2].to_int()] = _params[1];
    }
    else{
        _params[0].assign(_params[1]);
        env.get(_params[0].name) = _params[0];
    }
}

void evalStrung(unit & node, environment & env){
    std::string _stringVal = node.name;
    for(size_t count = 0; count < _stringVal.size(); count++){
        if(_stringVal[count] == '%'){
            size_t _stop = _stringVal.find('%', count + 1);
            std::string context = _stringVal.substr(count + 1, _stop - (count + 1) + 1);
            _units _sub = parseScript(context, env);
            _stringVal.erase(count,_stop - count + 1);
            _stringVal.insert(count, env.get(_sub[0].name)._childs[0].name);
        }
    }
    node.name = _stringVal;
}
