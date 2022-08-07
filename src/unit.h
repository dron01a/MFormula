#ifndef ____UNIT__HHH____
#define ____UNIT__HHH____

#include <iostream>

#include <string>
#include <vector>
#include <stack>

#define NPOS std::string::npos

struct unit;
class environment;

typedef std::vector<unit> unit_vector;
typedef std::stack<unit> unit_stack;

enum errors{
    SORCE_EMPTY
};

enum class _type{
    _opr = 0, 
    
    _func, 
    _coreFunc,  
    _list,
    
          
    _string,      
    _num, 
    _bool,
    _var,
    _continue,
    _break,
    
    
    _openBrt,      
    _closeBrt,

    _special,   
    _semicolon, 

    _varInit,
    _functionInit,
    _if,
    _else,
    _while,
    _for,
    _return,
    
    _indentf, 
    
    _include,

    _pointer,

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
    unit(long double _num);

    /**
     * class constructor
     * 
     * @param _val double value of unit
    */
    unit(bool _val);

    unit & operator[](int position);

    bool to_bool() const;
    long double to_double() const;
    int to_int() const;
    std::string to_string() const;

    void assign(unit unit);
    void print();
    int size();
    void resize(int _newsize);

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



    unit(){};
    ~unit(){};

    unit_vector _childs;
    std::string name;

    unit * __mem = nullptr;

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
    unit_vector & defined(); 
    
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
    unit_vector _defined {
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
    error(unit & _token, std::string _message) : _unit(_token), message(_message){};
    
    unit _unit;
    std::string message;
};

/**
 * operator >>
 * from input struct unit
 * 
 * @param _stream input stream 
 * @param unit unit 
 * 
*/
std::istream& operator>> (std::istream& _stream,unit & unit);

/**
 * operator <<
 * from input struct unit
 * 
 * @param _stream output stream 
 * @param unit unit 
 * 
*/
std::ostream& operator<< (std::ostream& _stream,unit & unit);


std::ostream& operator<< (std::ostream& _stream,error & _error);


#endif