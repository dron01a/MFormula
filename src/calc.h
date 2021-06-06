#ifndef CALCULATIONS_H_
#define CALCULATIONS_H_

#include <map>
#include <stack>
#include <string>
#include "math.h"

double calcUnits(std::stack<double> &args, std::string exp, int prior);

double factorial(double n); 

static double simpleFunc(std::string operation, double a);

static double binaryFunc(std::string operation, double a, double b);

static void setVars(std::stack<double> &args, double &a, double &b);

static void setVars(std::stack<double> &args, double &a);

#endif