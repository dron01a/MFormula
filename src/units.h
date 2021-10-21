#ifndef ____UNITS__H____
#define ____UNITS__H____

#include <string>
#include <string.h>
#include <algorithm>
#include <vector>
#include <stack>

#define NPOS std::string::npos

struct unit;
class environment;

typedef std::vector<unit> _units;

enum class _type{
    _num,       // numbers -> 0-9  
    _brt,       // brackets -> () [] 
    _opr,       // operations -> +-*/^%
    _coreFunc,  // functions -> abs sin cos tg ctg 
    _special,   // special chars -> e pi 
    _text,      // string text in "some text" 
    _semicolon, // ;
    _var,
    _list,
    _func,
    _comment,
    _varInit,
    _functionInit,
    _indentf 
};

struct unit{ 
    unit(_type t,std::string s);
    unit(_type t,std::string s, int pr);
    unit(){};
    _units _childs;
    std::string name;
    _type type;
    int prior = 0;
};

class environment {
public:
    environment();
    unit get(std::string _name);
    bool have(std::string _name);
    void add(unit & _unit);
private:
    _units _defined {
        unit(_type::_var,"pi"),
        unit(_type::_var,"e"),    
    };
};


#endif