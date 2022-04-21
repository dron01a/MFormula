#ifndef CALCULATIONS_H_
#define CALCULATIONS_H_

#include <map>
#include <stack>
#include <string>
#include "math.h"

typedef double(*simpleF)(double);
typedef double(*binaryF)(double, double);

// calculations 
double calcUnits(std::stack<double> &args, std::string exp, int prior);

// factorial function 
double factorial(double n); 

// set values to vars 
static void setVars(std::stack<double> &args, double &a, double &b);
static void setVars(std::stack<double> &args, double &a);

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

#endif