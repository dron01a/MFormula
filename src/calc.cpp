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

double calcUnits(std::stack<double> &args, std::string exp){
    if(args.size() >= 2){
        double a,b;
        setVars(args,a,b);
        if(exp == "+"){
            return a+b;
        }
        if(exp == "-"){
            return a-b;
        }
        if(exp == "/"){
            return (a/b);
        }
        if(exp == "*"){
            return (a*b);
        }
        if(exp == "^"){
            return pow(a,b);
        }
    }
    if(args.size() == 1){
        double a;
        setVars(args,a);
        if(exp == "sin"){
            return sin(a);
        }
        if(exp == "cos"){
            return cos(a);
        }
        if(exp == "tg"){
            return tan(a);
        }
        if(exp == "ctg"){
            return (1/tan(a));
        }
        if(exp == "abs"){
            return abs(a);
        }
    }
}