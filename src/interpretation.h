#ifndef __INTERPRT__H__
#define __INTERPRT__H__

#include <map>
#include "math.h"

#include "units.h"
#include "lexer.h"
#include "parser.h"

void varInit(unit & node, environment & env);

void funcInit(unit & node, environment & env);

void if_iterpr(unit & node, environment & env);

void whileInterpt(unit & node, environment & env);

void forInterpt(unit & node, environment & env);

void callFunc(unit & node, std::stack<unit> & _params,  environment & env);

void assign(std::stack<unit> & _params, environment & env);

void evalStrung(unit & node, environment & env);

void loop(Parser & _exprP, Parser & _condP, Parser & _stepP ,environment & _env, environment & _local);

void eval(_units & tokens, environment &env);

void evalList(unit & node, environment &env); 

//unit * getListChild(unit & node, environment & env);

unit & value(unit & node);

_units eval( Parser & _par, environment & env);

typedef unit(*simpleF)(unit);
typedef unit(*binaryF)(unit, unit);
typedef void(*memF)(std::stack<unit> &, environment & );
typedef void(*keyFunc)(unit &, environment & );

void set_in_env(std::stack<unit> & params, environment & env);

void decrem(std::stack<unit> & params, environment & env);

void increm(std::stack<unit> & params, environment & env);

// calculations 
unit calcUnits(std::stack<unit> &args, std::string exp, int prior);

// factorial function 
double factorial(double n); 

// set values to vars 
_units setVars(std::stack<unit> &args, int _count); 

_units rsetVars(std::stack<unit> &args, int _count); 

void run(std::string _script);

// return units from script
_units parseScript(std::string _script, environment & env);

static std::map<std::string, memF> memFuncs{
    {"[]", set_in_env},
    {"=", assign },
    {"++", increm },
    {"--", decrem },
    {"print", [](std::stack<unit> & params, environment & env){
        _units _params = setVars(params,params.size());
        for(size_t _par = 0; _par < _params.size(); _par++){
            if(value(_params[_par]).type == _type::_list){
                env.get(value(_params[_par]).name).print();
                continue;
            }
            value(_params[_par]).print();
        }
    }},
    {"size", [](std::stack<unit> & params, environment & env){
        _units _params = setVars(params,1);
        params.push(unit((double)value(_params[0]).size()));
    }},
    {"resize", [](std::stack<unit> & params, environment & env){
        _units _params = setVars(params,2);
        value(_params[0]).resize(value(_params[1]).to_int());
        //params.push(unit((double)value(_params[0]).resize()));
    }}
};

static std::map<_type, keyFunc> _keyWords{
    {_type::_varInit, varInit },
    {_type::_functionInit, funcInit },
    {_type::_if, if_iterpr },
    {_type::_for, forInterpt },
    {_type::_while, whileInterpt }
};

static std::map<std::string, simpleF> simpleFuncs{
    {"++",[](unit a){return unit(a.increment());}},
    {"--",[](unit a){return unit(a.decrement());}},
    {"nvar",[](unit a){return unit(-a.to_double());}},
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
    {"!=",[](unit a,unit b){return unit(a!=b);}},
    {">",[](unit a,unit b){return unit(a>b);}},
    {"<",[](unit a,unit b){return unit(a<b);}},
    {">=",[](unit a,unit b){return unit(a>=b);}},
    {"<=",[](unit a,unit b){return unit(a<=b);}},
    {"&&",[](unit a,unit b){return unit(a&&b);}},
    {"||",[](unit a,unit b){return unit(a||b);}},
    {"+",[](unit a,unit b){return a+b;}},
    {"-",[](unit a,unit b){return a-b;}},
    {"/",[](unit a,unit b){return a/b;}},
    {"*",[](unit a,unit b){return a*b;}},
    {"%",[](unit a,unit b){return a%b;}},
    {"^",[](unit a,unit b){return unit(pow(a.to_double(),b.to_double()));}},
    {"log",[](unit a,unit b){return unit((log(a.to_double())/log(b.to_double())));}}
};



struct _ctrlConst{
    _ctrlConst(environment _env, _type _t){
        env = _env;
        type = _t;
    } 
    ~_ctrlConst(){};
    environment env;
    _type type;
};

#endif