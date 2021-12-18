#ifndef ____UNITS__H____
#define ____UNITS__H____

#include <string>
#include <string.h>
#include <algorithm>
#include <vector>
#include <stack>

#define NPOS std::string::npos

class unit;
class environment;

typedef std::vector<unit> _units;

enum class _type{
    _num, 
    _bool,
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
    _if,
    _else,
    _while,
    _for,
    _continue,
    _break,
    _indentf 
};

class unit{
public:
    unit(_type t,std::string s, int pr = 0);
    unit(double _num);
    unit(bool _val);
    unit(){};
    ~unit(){};
    void assign(unit unit);
    void print();

    // math operators
    unit operator+(unit & _unit) const;
    unit operator-(unit & _unit) const;
    unit operator*(unit & _unit) const;
    unit operator/(unit & _unit) const;
    unit operator%(unit & _unit) const;
    unit increment();
    unit decrement();
    
    // compare operators
    bool operator==(const unit & _unit) const;
    bool operator!=(const unit & _unit) const;
    bool operator>(unit & _unit) const;
    bool operator<(unit & _unit) const;
    bool operator>=(unit & _unit) const;
    bool operator<=(unit & _unit) const;

    bool operator&&(unit & _unit) const;
    bool operator||(unit & _unit) const;

    bool to_bool() const;
    double to_double() const;
    int to_int() const;
    std::string to_string() const;

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
    void saveChange(environment & env);
private:
    _units _defined {
        unit(_type::_var,"pi"),
        unit(_type::_var,"e"),    
    };
};


#endif