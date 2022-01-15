#ifndef __PARCER__H__
#define __PARCER__H__

#include "units.h"

typedef bool(*condFunc)(unit, std::stack<unit>&);

// test name 
_units getUnits(_units & units,int _begin, int _end);

class Parser{
public:
    Parser(_units & units, environment & env);
    _units getTokens();
private:
    void getUnitsIn(std::stack<unit> & oprStack, unit curUnit, condFunc func);
    void parseCloseBrt(std::stack<unit> & oprStack, unit curUnit, char _type);
    int checkCloseBrt(_units & units, int position);
    _units _tokens; 



    void parseVarInit(_units & units,environment & env, int & count);
    void parseListInit(unit & newUnit, _units & units,environment & env, int & count);
    void parseCondition(_units & units,environment & env, int & count);
    unit parseFuncInit(_units & units,environment & env, int & count);
    unit parseIF(_units & units,environment & env, int & count);
    unit parseWhile(_units & units,environment & env, int & count);
    unit parseFor(_units & units,environment & env, int & count);
};

#endif