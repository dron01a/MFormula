#ifndef __PARCER__H__
#define __PARCER__H__

#include "units.h"

typedef bool(*condFunc)(unit, std::stack<unit>&);

// test name 
//_units getUnits(_units & units, );

class Parcer{
public:
    Parcer(_units & units, environment & env);
    _units getTokens();
private:
    void getUnitsIn(std::stack<unit> & oprStack, unit curUnit, condFunc func);
    void parceCloseBrt(std::stack<unit> & oprStack, unit curUnit, char _type);
    bool checkCloseBrt(_units & units, int position);
    _units _tokens; 
};

#endif