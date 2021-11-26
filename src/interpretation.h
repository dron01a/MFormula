#ifndef __INTERPRT__H__
#define __INTERPRT__H__

#include <map>
#include "math.h"

#include "units.h"
#include "lexer.h"
#include "parcer.h"

void varInit(unit & node, environment & env);

void eval(_units & tokens, environment &env);

typedef unit(*simpleF)(unit);
typedef unit(*binaryF)(unit, unit);

// calculations 
unit calcUnits(std::stack<unit> &args, std::string exp, int prior);

// factorial function 
double factorial(double n); 

// set values to vars 
void setVars(std::stack<unit> &args, unit &a, unit &b);
void setVars(std::stack<unit> &args, unit &a);

void run(std::string _script);

static std::map<std::string, simpleF> simpleFuncs{
    {"sin",[](unit a){return unit(sin(a.to_double()));}},
    {"cos",[](unit a){return unit(cos(a.to_double()));}},
    {"tg",[](unit a){return unit(tan(a.to_double()));}},
    {"ctg",[](unit a){return unit((1/tan(a.to_double())));}},
    {"arcsin",[](unit a){return unit(asin(a.to_double()));}},
    {"arccos",[](unit a){return unit(acos(a.to_double()));}},
    {"arctg",[](unit a){return unit(atan(a.to_double()));}},
    {"arcctg",[](unit a){return unit((atan(-a.to_double())+M_PI_2));}},
    {"abs",[](unit a){return unit(abs(a.to_double()));}},
    {"ln",[](unit a){return unit(log(a.to_double()));}},
    {"deg",[](unit a){return unit((a.to_double()*M_PI/180));}},
    {"sqrt",[](unit a){return unit(pow(a.to_double(),(1/2)));}},
    {"!",[](unit a){return unit(factorial(a.to_double()));}}
};

static std::map<std::string, binaryF> binaryFuncs{
    {"==",[](unit a,unit b){return unit(a==b);}},
    {"+",[](unit a,unit b){return a+b;}},
    {"-",[](unit a,unit b){return a-b;}},
    {"/",[](unit a,unit b){return a/b;}},
    {"*",[](unit a,unit b){return a*b;}},
    {"%",[](unit a,unit b){return a%b;}},
    {"^",[](unit a,unit b){return unit(pow(a.to_double(),a.to_double()));}},
    {"log",[](unit a,unit b){return unit((log(a.to_double())/log(a.to_double())));}}
};

#endif