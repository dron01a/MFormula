#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include "math.h"

#include "lexer.h"
#include "parcer.h"

void varInit(unit & node, environment & env);

void eval(_units & tokens, environment &env);

typedef double(*simpleF)(double);
typedef double(*binaryF)(double, double);

// calculations 
double calcUnits(std::stack<unit> &args, std::string exp, int prior);

// factorial function 
double factorial(double n); 

// set values to vars 
static void setVars(std::stack<unit> &args, double &a, double &b);
static void setVars(std::stack<unit> &args, double &a);

static std::map<std::string, simpleF> simpleFuncs{
    {"sin",[](double a){return sin(a);}},
    {"cos",[](double a){return cos(a);}},
    {"tg",[](double a){return tan(a);}},
    {"ctg",[](double a){return (1/tan(a));}},
    {"arcsin",[](double a){return asin(a);}},
    {"arccos",[](double a){return acos(a);}},
    {"arctg",[](double a){return atan(a);}},
    {"arcctg",[](double a){return (atan(-a)+M_PI_2);}},
    {"abs",[](double a){return abs(a);}},
    {"ln",[](double a){return log(a);}},
    {"deg",[](double a){return (a*M_PI/180);}},
    {"sqrt",[](double a){return pow(a,(1/2));}},
    {"!",[](double a){return factorial(a);}}
};

static std::map<std::string, binaryF> binaryFuncs{
    {"+",[](double a,double b){return a+b;}},
    {"-",[](double a,double b){return a-b;}},
    {"/",[](double a,double b){return a/b;}},
    {"*",[](double a,double b){return a*b;}},
    {"^",[](double a,double b){return pow(a,b);}},
    {"%",[](double a,double b){return double((int)a%(int)b);}},
    {"log",[](double a,double b){return (log(b)/log(a));}}
};


int main(int argc, char *argv[]){
    if(argc <= 1 ){
        std::cout << "not valid args";
    }
    if(strcmp(argv[1],"-s") == 0){
        environment env;
        Lexer lex(std::string(argv[2]), env);
        _units u = lex.getUnits();
        Parcer par(u,env);
        u = par.getTokens();
        eval(u,env);
        std::cout << u[0].name << std::endl;
    }
    else if(strcmp(argv[1],"-f") == 0){
        std::ifstream in( argv[2],std::ios::binary);
        if(!in.good()){
            std::cout << "not found file " << argv[2] << std::endl;
        }
        int size = in.seekg(0,std::ios::end).tellg();
        in.seekg(0);
        char * buf = new char[size+1];
        in.read(buf,size);
        buf[size] = 0;
        environment env;
        Lexer lex(std::string(buf), env);
        _units u = lex.getUnits();
        Parcer par(u,env);
        u = par.getTokens();
        eval(u,env);
    }
    return 0; 
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
            params.push(env.get(tokens[count].name)._childs[0]);
            break;
        case _type::_opr:
        case _type::_coreFunc:
            if(tokens[count].name == "print"){
                std::cout << params.top().name << std::endl;
                params.pop();
                continue;
            }
            params.push(unit(_type::_num,std::to_string(calcUnits(params,tokens[count].name,tokens[count].prior))));
            break;
        }
    }
    tokens.clear();
    if(params.size() != 0){
        tokens.push_back(params.top());
    }
}

void setVars(std::stack<unit> &args, double &a,  double &b){
    b = std::strtod(args.top().name.c_str(),nullptr);
    args.pop();
    a = std::strtod(args.top().name.c_str(),nullptr);
    args.pop();
}

void setVars(std::stack<unit> &args, double &a){
    a = std::strtod(args.top().name.c_str(),nullptr);
    args.pop();
}

double calcUnits(std::stack<unit> &args, std::string exp, int prior){
    if(args.size() == 0){
        throw std::string("operation:" + exp +" --> no arguments!");
    }
    if((prior > 0 && exp != "!") || exp == "log"){
        double a,b;
        if(args.size() == 1){
            setVars(args,a);
            b=a;
            a=0;
        }
        else{
            setVars(args,a,b);
        }
        return binaryFuncs[exp](a,b);
    }
    else{
        double a;
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