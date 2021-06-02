#include "calc.h"

void setVars(std::stack<double> &args, double &a,  double &b){
    b = args.top();
    args.pop();
    a = args.top();
    args.pop();
}

void setVars(std::stack<double> &args, double &a){
    a = args.top();
    args.pop();
}

double simpleFunc(std::string operation, double a){
    if(operation == "sin"){
        return sin(a);
    }
    if(operation == "arcsin"){
        return asin(a);
    }
    else if(operation == "cos"){
        return cos(a);
    }
    if(operation == "arccos"){
        return acos(a);
    }
    else if(operation == "tg"){
        return tan(a);
    }
    if(operation == "arctg"){
        return atan(a);
    }
    else if(operation == "ctg"){
        return (1/tan(a));
    }
    if(operation == "arcctg"){
        return (1/atan(a));
    }
    else if(operation == "abs"){
        return abs(a);
    }
    else{
        throw std::string("unknown function: ") += operation; 
    }
}

double binaryFunc(std::string operation, double a, double b){
    if(operation == "+"){
        return a+b;
    }
    if(operation == "-"){
        return a-b;
    }
    if(operation == "/"){
        return (a/b);
    }
    if(operation == "*"){
        return (a*b);
    }
    if(operation == "^"){
        return pow(a,b);
    }
    else{
        throw std::string("unknown function: ") += operation; 
    }
}

double calcUnits(std::stack<double> &args, std::string exp, int prior){
    if(args.size() == 0){
        throw std::string("operation:" + exp +" --> no arguments!");
    }
    if(prior > 0){
        double a,b;
        if(args.size() == 1){
            setVars(args,a);
            b=a;
            a=0;
        }
        else{
            setVars(args,a,b);
        }
        return binaryFunc(exp,a,b);
    }
    else{
        double a;
        setVars(args,a);
        return simpleFunc(exp,a);
    }
}