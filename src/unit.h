#ifndef ____UNIT__HHH____
#define ____UNIT__HHH____

#include <string>
#include <vector>
#include <stack>

#define NPOS std::string::npos

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
    _var,
    _list,
    _func,
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
    _include,
    _classInit,
    _classVar,
    _classFV,
    _classFM
};

struct unit{
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

    _type type = _type::_indentf;
    size_t prior = 0;
    size_t _str = 0;
    size_t _col = 0;
    
};

class environment {
public:

    /**
     * constructor
     *  
     * @param env environment of code 
     * 
    */
    environment(environment & env);
    
    // constructor
    environment();
    
    /**
     * get unit in environment 
     * 
     * @param _name name of needed unit 
     * 
     * @return unit from name
    */     
    unit & get(std::string _name);
    
    // return all defined units
    _units & defined(); 
    
    /**
     * 
     * check unit in environment 
     *  
     * @param _name name of needed unit 
     *
     * @return true or false 
    */
    bool have(std::string _name);

    /**
     * add unit toin environment 
     * 
     * @param _unit new unit 
    */     
    void add(unit & _unit);
    
    // combuned invironment
    void comb(environment & env);

    void save_change(environment & env);
private:
    // defined var`s and functions
    _units _defined {
        unit(_type::_var,"pi"),
        unit(_type::_var,"e"),    
    };
};

struct error{
    /**
     * class constructor
     * 
     * @param _token token with error 
     * @param _message message with text of error 
    */
    error(unit & _token, std::string _message) : _unit(_unit), message(_message){};
    
    unit _unit;
    std::string message;
};


#endif