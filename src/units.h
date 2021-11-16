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
    _num, 
    _openBrt,      
    _closeBrt,      
    _opr,       
    _coreFunc,  
    _special,   
    _text,      
    _semicolon, 
    _newLine,
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
    unit(double _num) ;
    unit(){};
    void assign(unit unit);
    void print();
    unit operator+(unit & _unit) const;
    unit operator-(unit & _unit) const;
    unit operator*(unit & _unit) const;
    unit operator/(unit & _unit) const;
    unit operator%(unit & _unit) const;
    operator double() const;
    _units _childs;
    std::string name;
    _type type;
    int prior = 0;
};

class environment {
public:
    environment();
    environment(environment & env);
    unit & get(std::string _name);
    _units & defined();
    bool have(std::string _name);
    void add(unit & _unit);
private:
    _units _defined {
        unit(_type::_var,"pi"),
        unit(_type::_var,"e"),    
    };
};


#endif