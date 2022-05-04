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

void evalList(unit & node, environment & env){
    for(size_t count = 0; count < node._childs.size(); count++){
        if(node[count].type == _type::_list){
            evalList(node[count], env);
            continue;
        }
        Parser pars(node[count]._childs, env);
        node[count] = eval(pars, env)[0];
    }
}

void varInit(unit & node, environment & env){
    for(size_t _vars = 0;_vars < node._childs.size(); _vars++ ){
        _units childs = env.get(node._childs[_vars].name)._childs;  
        if(env.get(node._childs[_vars].name).type == _type::_list){
            unit listNode;
            listNode = env.get(node._childs[_vars].name);
            evalList(listNode, env);
            env.get(node._childs[_vars].name) = listNode;
        }
        else{
            if(childs.size() != 0){
                Parser childsParcer(childs,env);
                childs = eval(childsParcer, env); 
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
        case _type::_functionInit:
        case _type::_if:
        case _type::_for:
        case _type::_while:
            _keyWords[tokens[count].type](tokens[count], _local);
            break;
        case _type::_string:
            evalStrung(tokens[count],_local);
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
            if(memFuncs.count(tokens[count].name) != 0){
                memFuncs[tokens[count].name](params,_local);
                continue;
            }
            params.push(calcUnits(params,tokens[count].name,tokens[count].prior));
            break;
        case _type::_func:
            callFunc(tokens[count],params,_local);
            break;
        case _type::_break:  
        case _type::_continue:
            {
                _ctrlConst _thr(_local,tokens[count].type); 
                throw _thr;
            }
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
        }
    }
    env.save_change(_local);
    tokens.clear();
    if(params.size() != 0){
        tokens.push_back(value(params.top()));
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

_units rsetVars(std::stack<unit> &args, int _count){
    _units result;
    _units tmp = setVars(args, args.size());
    for(int i = 0; i < _count; i++){
        result.push_back(tmp[i]);
    }
    for(int i = _count; i != tmp.size(); i++){
        args.push(tmp[i]);
    }
    return result;
}

unit calcUnits(std::stack<unit> &args, std::string exp, int prior){
    if(args.size() == 0){
        throw std::string("operation:" + exp +" --> no arguments!");
    }
    if((prior > 0  && exp != "!" && exp != "nvar") || exp == "log" || exp == "&&" || exp == "||"){
        _units _params = setVars(args,2);
        return binaryFuncs[exp](value(_params[0]),value(_params[1]));
    }
    else{
        _units _params = setVars(args,1);
        return simpleFuncs[exp](value(_params[0]));//simpleFunc(exp,a);
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
    _units cond = eval(_condParce,env);
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
    env.save_change(_local);
}

void loop(Parser & _exprP, Parser & _condP, Parser & _stepP, environment & _global, environment & _local){
    _units cond = eval(_condP,_local);
    _units step;
    while(cond[0].to_bool() == true){
        try{
            _units expr = _exprP.getTokens();
            eval(expr,_local);
        }
        catch(_ctrlConst _T){
            _local.save_change(_T.env);
            _global.save_change(_local);
            if(_T.type == _type::_continue){
                step = eval(_stepP,_local);
                cond = eval(_condP,_local);
                continue;
            }  
            if(_T.type == _type::_break){
                cond[0].name = "false";
                break;
            }
        }
        step = eval(_stepP,_local);
        cond = eval(_condP,_local);
    }
    _global.save_change(_local);
}

void whileInterpt(unit & node, environment & env){
    environment _local(env);
    Parser _condParce(node._childs[0]._childs,env);
    Parser _exprParce(node._childs[1]._childs,_local);
    loop(_exprParce, _condParce, _condParce, env, _local);
}

void forInterpt(unit & node, environment & env){
    environment _local(env);
    Parser _initParce(node._childs[0]._childs,_local);
    Parser _condParce(node._childs[1]._childs,_local);
    Parser _stepParce(node._childs[2]._childs,_local);
    Parser _exprParce(node._childs[3]._childs,_local);
    _units init = eval(_initParce,_local);
    loop(_exprParce, _condParce, _stepParce, env, _local);  
}

void callFunc(unit & node, std::stack<unit> & _prms, environment & env){
    environment _local; 
    unit _func = env.get(node.name);
    _units _params = _func._childs[0]._childs;
    _units _expr = _func._childs[1]._childs;
    Parser _parsParm(_params, _local);
    _params = _parsParm.getTokens();
    _units _functionPar = setVars(_prms, _params[0]._childs.size());
    _local.comb(env);
    for(int _parC = 0; _parC < _functionPar.size(); _parC++){
        _local.defined()[_parC+2].assign(value(_functionPar[_parC]));
    }
    Parser _parsExpr(_expr, _local);
    _expr = eval(_parsExpr, _local);  
    if(_expr.size() != 0){
        _prms.push(value(_expr[0]));
    }
}

void assign(std::stack<unit> & params, environment & env){
    _units _params = setVars(params,2);
    value(_params[0]).assign(value(_params[1]));
}

unit & value(unit & node){
    if(node.__mem != nullptr){
        return *node.__mem;
    }
    return node;
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
        if(_stringVal[count] == '\\'){
            if(_stringVal[count + 1] == 'n'){
                _stringVal[count] = '\n';
                _stringVal.erase(count+1,1);
            }
        }
    }
    node.name = _stringVal;
}

void set_in_env(std::stack<unit> & params, environment & env){
    _units _params = setVars(params,2);
    unit _temp;
    if(_params[0].__mem == nullptr){
        _temp.__mem = & env.get(_params[0].name)._childs[value(_params[1]).to_int()];
    }
    else{
        _temp.__mem = & _params[0].__mem->_childs[value(_params[1]).to_int()];
    }
    params.push(_temp);
    
}

void decrem(std::stack<unit> & params, environment & env){
    if(value(params.top()).type == _type::_var ){
        value(params.top())[0] = value(params.top())[0].decrement();
    }
    if(value(params.top()).type == _type::_num ){
        value(params.top()) = value(params.top()).decrement();
    }
    params.pop();
}


void increm(std::stack<unit> & params, environment & env){
    if(value(params.top()).type == _type::_var ){
        value(params.top())[0] = value(params.top())[0].increment();
    }
    if(value(params.top()).type == _type::_num ){
        value(params.top()) = value(params.top()).increment();
    }
    params.pop();
}

