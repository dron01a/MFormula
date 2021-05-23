#ifndef CALCULATIONS_H_
#define CALCULATIONS_H_

#include <stack>
#include <string>
#include "math.h"

double calcUnits(std::stack<double> &args, std::string exp);

void setVars(std::stack<double> &args, double &a, double &b);

void setVars(std::stack<double> &args, double &a);

#endif