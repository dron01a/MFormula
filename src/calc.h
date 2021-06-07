#ifndef CALCULATIONS_H_
#define CALCULATIONS_H_

#include <map>
#include <stack>
#include <string>
#include "math.h"

// calculations 
double calcUnits(std::stack<double> &args, std::string exp, int prior);

// factorial function 
double factorial(double n); 

// for functions with 1 param 
static double simpleFunc(std::string operation, double a);

// for functions with 1 param 
static double binaryFunc(std::string operation, double a, double b);

// set values to vars 
static void setVars(std::stack<double> &args, double &a, double &b);
static void setVars(std::stack<double> &args, double &a);

#endif