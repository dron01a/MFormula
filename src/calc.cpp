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
        double a,b;
        if(exp == "+"){
            setVars(args,a,b);
            return a+b;
        }
        if(exp == "-"){
            if(args.size() == 1){
                if(exp == "-"){
                    setVars(args,a);
                    return -a;
                }
            }
            setVars(args,a,b);
            return a-b;
        }
        if(exp == "/"){
            setVars(args,a,b);
            return (a/b);
        }
        if(exp == "*"){
            setVars(args,a,b);
            return (a*b);
        }
        if(exp == "^"){
            setVars(args,a,b);
            return pow(a,b);
        }
        if(exp == "sin"){
            setVars(args,a);
            return sin(a);
        }
        if(exp == "cos"){
            setVars(args,a);
            return cos(a);
        }
        if(exp == "tg"){
            setVars(args,a);
            return tan(a);
        }
        if(exp == "ctg"){
            setVars(args,a);
            return (1/tan(a));
        }
        if(exp == "abs"){
            setVars(args,a);
            return abs(a);
        }
}