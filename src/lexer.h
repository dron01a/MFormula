#ifndef __LEXER__H__
#define __LEXER__H__

#include "units.h"

typedef bool(*compareFunc)(std::string, char);

static std::string openBrt = "[{(";
static std::string closeBrt = ")}]";
static std::string operators = "+-*/^%<>=";
static std::string special = ",";         
static std::string numbers = "-0123456789.";
static std::vector<std::string> functions {
    "abs", "arc", "cos", "sin", "tg", "ln", "ctg", "sqrt",
    "!", "log", "deg", "print", "nvar", "size",  "resize"
};

class Lexer{
public:
    Lexer(std::string source, environment & env);
    _units getUnits();
private:
    std::string _source;
    std::string _delim = "|&!{}[]()+-*/^%,;<>=\n";
    _units units;
    _type getType(std::string exp); // return type of expression 
    void addToToken(std::string & _token, int & _count, compareFunc _func);
    int getPriority(std::string _s);
};



#endif 