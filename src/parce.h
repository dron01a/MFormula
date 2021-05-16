#ifndef PARCE_H_
#define PARCE_H_

#include <string>
#include <string.h>
#include <vector>

#define NPOS std::string::npos

enum exmpElements{
    _num,    // numbers -> 0-9  
    _brt,    // brackets -> () [] 
    _opr,    // operations -> +-*/^%
    _func,   // functions -> abs sin cos tg ctg 
    _special // special chars -> e pi 
};

struct unit{
    std::string exp;
    exmpElements type;
    int prior = 0;
    unit(exmpElements t,std::string s):type(t), exp(s) {}
};


typedef std::vector<std::pair<exmpElements, char>> exmpChars;

typedef std::vector<unit> exmpUnits;

// return type of character 
exmpElements getCharType(std::string source, size_t position);

// parce example
exmpChars parceExmpl(std::string source);

// collect chars
exmpUnits parceChars(exmpChars & _chars);

// distribute char depending on the type of char
void distChars(exmpUnits & _units,exmpElements elem, exmpElements curElem, char curChar, int & position); 

// throw unknown char
void unknown(char c);

#endif