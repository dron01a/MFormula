#ifndef __PARCER__H__
#define __PARCER__H__

#include "units.h"

typedef bool(*condFunc)(unit, std::stack<unit>&);

// test name 
_units getUnits(_units & units,int _begin, int _end);

class Parcer{
public:
    Parcer(_units & units, environment & env);
    _units getTokens();
private:
    void getUnitsIn(std::stack<unit> & oprStack, unit curUnit, condFunc func);
    void parceCloseBrt(std::stack<unit> & oprStack, unit curUnit, char _type);
    int checkCloseBrt(_units & units, int position);
    _units _tokens; 

    void parceVarInit(_units & units,environment & env, int & count);
    void parceListInit(unit & newUnit, _units & units,environment & env, int & count);
    unit parceIF(_units & units,environment & env, int & count);
    unit parceWhile(_units & units,environment & env, int & count);
};

#endif