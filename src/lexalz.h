#ifndef __LEXALZ__
#define __LEXALZ__

#include <fstream>

#include "unit.h"

#define NPOS std::string::npos

static std::string delim = "|&!{}[]()+-*/^%,;<>=\n";
static std::string openBrt = "[{(";
static std::string closeBrt = ")}]";
static std::string operators = "+-*/^%<>=";
static std::string special = ",";         
static std::string numbers = "-0123456789.";
static std::vector<std::string> functions {
    "abs", "arc", "cos", "sin", "tg", "ln", "ctg", "sqrt",
    "!", "log", "deg", "print", "nvar", "size",  "resize",
    "system", "run", "exit"
};

typedef bool(*compareFunc)(std::string, char);

/**
    @param _source string with code
    @param _str number of string
    @return vector with code unit
*/
unit_vector lex(std::string _source, int _str = 0);

/**
 *  Get type of token
 *  @param _token string with token
 *  @return type of token
*/
static _type get_type(std::string _token);

/**
 * Get priority of token
 *  @param _token string with token
 *  @return int value priority
*/
static size_t get_priority(std::string & _token);

/**
 *  Add chars to token depending on the result of the function
 *  @param _source string with code
 *  @param _token string with token 
 *  @param _count num of currient character
 *  @param _func pointer to function    
*/
static void add_to_token(std::string & _source, std::string & _token, int & _count, compareFunc _func);

#endif