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
    Parcer childsParcer(childs,env);
    childs = childsParcer.getTokens();
    eval(childs, env);
    env.get(node._childs[0].name)._childs = childs;
}

void eval(_units & tokens,environment &env){
    std::stack<unit> params; 
    for(int count = 0; count < tokens.size(); count++){
        switch (tokens[count].type){
        case _type::_varInit:
            varInit(tokens[count],env);
            break;  
        case _type::_num:
        case _type::_text:
            params.push(tokens[count]);
            break;
        case _type::_var:
            params.push(env.get(tokens[count].name));
            break;
        case _type::_opr:
           if(tokens[count].name == "="){
               unit _a,_b;
               setVars(params,_a,_b);
               _a.assign(_b);
               env.get(_a.name) = _a;
               continue;
           }
        case _type::_coreFunc:
            if(tokens[count].name == "print"){
                params.top().print();
                params.pop(); 
                continue;
            }
            params.push(unit(calcUnits(params,tokens[count].name,tokens[count].prior)));
            break;
        }
    }
    tokens.clear();
    if(params.size() != 0){
        tokens.push_back(params.top());
    }
}

void setVars(std::stack<unit> &args, unit &a, unit &b){
    setVars(args,b);
    setVars(args,a);
}

void setVars(std::stack<unit> &args, unit &a){
    a = args.top();
    args.pop();
}

double calcUnits(std::stack<unit> &args, std::string exp, int prior){
    if(args.size() == 0){
        throw std::string("operation:" + exp +" --> no arguments!");
    }
    if((prior > 0 && exp != "!") || exp == "log"){
        unit a,b;
        if(args.size() == 1){
            setVars(args,a);
            b=a;
            a.name = "0";
        }
        else{
            setVars(args,a,b);
        }
        return binaryFuncs[exp](a,b);
    }
    else{
        unit a;
        setVars(args,a);
        return simpleFuncs[exp](a);//simpleFunc(exp,a);
    }
}

double factorial(double n){
    if(n==0){
        return 1;
    }
    return n*factorial(n-1);
}

