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