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
    if(operation == "!"){
        return factorial(a);
    }
    else if(operation == "ln"){
        return log(a);
    }
    else if(operation == "sin"){
        return sin(a);
    }
    else if(operation == "arcsin"){
        return asin(a);
    }
    else if(operation == "cos"){
        return cos(a);
    }
    else if(operation == "arccos"){
        return acos(a);
    }
    else if(operation == "tg"){
        return tan(a);
    }
    else if(operation == "arctg"){
        return atan(a);
    }
    else if(operation == "ctg"){
        return (1/tan(a));
    }
    else if(operation == "arcctg"){
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
    if(operation == "log"){
        return (log(b)/log(a));
    }
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
        return binaryFunc(exp,a,b);
    }
    else{
        double a;
        setVars(args,a);
        return simpleFunc(exp,a);
    }
}

double factorial(double n){
    if(n==0){
        return 1;
    }
    return n*factorial(n-1);
}