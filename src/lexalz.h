#ifndef __LEXALZ__
#define __LEXALZ__

#include "unit.h"

#define NPOS std::string::npos

static std::string _delim = "|&!{}[]()+-*/^%,;<>=\n";
static std::string openBrt = "[{(";
static std::string closeBrt = ")}]";
static std::string operators = "+-*/^%<>=";
static std::string special = ",";         
static std::string numbers = "-0123456789.";
static std::vector<std::string> functions {
    "abs", "arc", "cos", "sin", "tg", "ln", "ctg", "sqrt",
    "!", "log", "deg", "print", "nvar", "size",  "resize"
};

typedef bool(*compareFunc)(std::string, char);

/**
    @param _sorce string with code
    @param env environment of script
    @return vector with code unit
*/
_units lex(std::string & _sorce, environment & env);

/**
 *  Get type of token
 *  @param _token string with token
 *  @param env environment of script
 *  @return type of token
*/
static _type get_type(std::string & _token, environment & env);

/**
 * Get priority of token
 *  @param _token string with token
 *  @return int value priority
*/
static int get_priority(std::string & _token);

/**
 *  Add chars to token depending on the result of the function
 *  @param _token string with token 
 *  @param _count num of currient character
 *  @param _func pointer to function    
*/
static void add_to_token(std::string & _token, int & _count, compareFunc _func);

#endif