#ifndef PARCE_H_
#define PARCE_H_

#include <string>
#include <string.h>
#include <algorithm>
#include <vector>
#include <stack>

#define NPOS std::string::npos

enum exmpElements{
    _num,     // numbers -> 0-9  
    _brt,     // brackets -> () [] 
    _opr,     // operations -> +-*/^%
    _func,    // functions -> abs sin cos tg ctg 
    _special, // special chars -> e pi 
};

struct unit{
    std::string exp;
    exmpElements type;
    int prior = 0;
    unit(exmpElements t,std::string s):type(t), exp(s) {}
    unit(exmpElements t,std::string s, int pr):type(t), exp(s), prior(pr) {}
    unit(){};
};

typedef std::vector<unit> exmpUnits;

// parce expression
exmpUnits parce(std::string exmple);

// return type of expression term
exmpElements getType(std::string exp);

// throw error message 
void unknown(std::string c);

// get priority of operation 
int getPriority(std::string exp);

// finds closing brackets
int findCloseBrt(exmpUnits & units, int position);

// make postfix queue
exmpUnits creatrePostfix(exmpUnits & _units);

// set data in stack for brt
void getUnitsIn(std::string obj ,exmpUnits & _units, std::stack<unit> & oprStack);

typedef bool(*condFunc)(unit, std::stack<unit>&);

void getUnitsIn(exmpUnits & _units, std::stack<unit> & oprStack, unit curUnit, condFunc func);

#endif