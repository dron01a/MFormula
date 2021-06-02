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
    _uncn
};

struct unit{
    std::string exp;
    exmpElements type;
    int prior = 0;
    unit(exmpElements t,std::string s):type(t), exp(s) {}
    unit(exmpElements t,std::string s, int pr):type(t), exp(s), prior(pr) {}
    unit(){};
};

typedef std::vector<std::pair<exmpElements, char>> exmpChars;

typedef std::vector<unit> exmpUnits;

// return type of character 
exmpElements getCharType(std::string source, size_t position);

bool findExp(std::string exp, std::string source, int position);

// parce example
exmpChars parceExmpl(std::string source);

// collect chars
exmpUnits parceChars(exmpChars & _chars);

int getPriority(std::string exp);

int findCloseBrt(exmpUnits & units, int position);

// distribute char depending on the type of char
void distChars(exmpUnits & _units,exmpElements elem, exmpElements curElem, char curChar, int & position); 

// make 
exmpUnits creatrePostfix(exmpUnits & _units);

void getUnitsIn(std::string obj ,exmpUnits & _units, std::stack<unit> & oprStack);

// throw unknown char
void unknown(char c);

#endif