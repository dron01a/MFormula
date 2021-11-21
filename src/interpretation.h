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
    {"sin",[](unit a){return unit(sin(double(a)));}},
    {"cos",[](unit a){return unit(cos(double(a)));}},
    {"tg",[](unit a){return unit(tan(double(a)));}},
    {"ctg",[](unit a){return unit((1/tan(double(a))));}},
    {"arcsin",[](unit a){return unit(asin(double(a)));}},
    {"arccos",[](unit a){return unit(acos(double(a)));}},
    {"arctg",[](unit a){return unit(atan(double(a)));}},
    {"arcctg",[](unit a){return unit((atan(-double(a))+M_PI_2));}},
    {"abs",[](unit a){return unit(abs(double(a)));}},
    {"ln",[](unit a){return unit(log(double(a)));}},
    {"deg",[](unit a){return unit((double(a)*M_PI/180));}},
    {"sqrt",[](unit a){return unit(pow(double(a),(1/2)));}},
    {"!",[](unit a){return unit(factorial(double(a)));}}
};

static std::map<std::string, binaryF> binaryFuncs{
    {"+",[](unit a,unit b){return a+b;}},
    {"-",[](unit a,unit b){return a-b;}},
    {"/",[](unit a,unit b){return a/b;}},
    {"*",[](unit a,unit b){return a*b;}},
    {"^",[](unit a,unit b){return unit(pow(double(a),double(a)));}},
    {"%",[](unit a,unit b){return unit(double((int)a%(int)b));}},
    {"log",[](unit a,unit b){return unit((log(double(a))/log(double(a))));}}
};

#endif