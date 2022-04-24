#ifndef ____UNIT__HHH____
#define ____UNIT__HHH____

#include <string>
#include <vector>

struct unit;
class environment;

typedef std::vector<unit> _units;

enum errors{
    SORCE_EMPTY
};

enum class _type{
    _num, 
    _bool,
    _openBrt,      
    _closeBrt,      
    _opr,       
    _coreFunc,  
    _special,   
    _string,      
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
    _return,
    _indentf, 
    _memOpr,
    _sqrBrtOpen,
    _sqrBrtClose,
    _include
};

struct unit{
public:
    /**
     * class constructor
     * 
     * @param t type of code unit
     * @param s string value
     * @param pr priority of unit
     * 
    */
    unit(_type t,std::string s, int pr = 0);

    /**
     * class constructor
     * 
     * @param _num double value of unit
    */
    unit(double _num);

    /**
     * class constructor
     * 
     * @param _val double value of unit
    */
    unit(bool _val);

    unit(){};
    ~unit(){};

    _units _childs;
    std::string name;

    unit * __mem = nullptr;

    typedef union _data{
        double float_data;
        long double long_data;
        std::string str_data; 
    } _unitData;

    _type type;
    int prior = 0;
    int _str = 0;
    int _col = 0;
    
};

class environment {
public:
    environment();
    environment(environment & env);
    unit & get(std::string _name);
    _units & defined();
    bool have(std::string _name);
    void add(unit & _unit);
    void comb(environment & env);
    void saveChange(environment & env);
};



#endif