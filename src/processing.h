#ifndef __PROC__H__
#define __PROC__H__

#include "unit.h"
#include "parse.h"
#include "fileio.h"

#include "math.h"

typedef unit(*simple_func)(unit);
typedef unit(*binary_func)(unit, unit);
typedef void(*memF)(unit_stack &, environment & );
typedef void(*key_func)(unit &, environment & );

/**
 * processes parts of the code *
 * clear and edit tokens !!! 
 * 
 * @param tokens vector with units 
 * @param env envinronment of run code
 * 
*/
void eval(unit_vector & tokens,environment &env);

/**
 * set units in stack from vector 
 * 
 * @param args stack with units 
 * @param _count number of needed units 
 * 
 * @return vector with units  
*/
unit_vector set_vars(unit_stack &args, int _count); 

/**
 * give access to unit 
 * 
 * @param node needed node
 * 
 * @return self from unit
*/
unit & value(unit & node);

/**
 * from operator []
 * 
 * @param params stack with units 
 * @param env envinronment of run code
 * 
*/
void set_in_env(unit_stack & params, environment & env);


/**
 * processing operator { var }
 * 
 * @param node needed node
 * @param env envinronment of run code
*/
void var_init(unit & node, environment & env);


/**
 * processing operator { func }
 * 
 * @param node needed node
 * @param env envinronment of run code
*/
void func_init(unit & node, environment & env);


/**
 * processing lists
 * 
 * @param node node with list information
 * @param env envinronment of run code
*/
void eval_list(unit & node, environment &env); 

/**
 * processing if
 * 
 * @param node node with if construction
 * @param env envinronment of run code
*/
void if_iterpr(unit & node, environment & env);

/**
 * processing while loop
 * 
 * @param node node with while construction
 * @param env envinronment of run code
*/
void while_interpt(unit & node, environment & env);

/**
 * processing for loop
 * 
 * @param node node with for construction
 * @param env envinronment of run code
*/
void for_interpt(unit & node, environment & env);

/**
 * processing loops 
 * 
 * @param _expr code of loop  
 * @param _cond condition of loop 
 * @param _step step of loop 
 * @param env envinronment of run code
*/
void loop(unit_vector & _expr, unit_vector & _cond, unit_vector & _step ,environment & _env, environment & _local);

/**
 * Assign 
 * 
 * @param _params stack with parametrs 
 * @param env envinronment of run code
 * 
*/
void assign(unit_stack & _params, environment & env);

/**
 * decrem 
 *
 * @param params stack with parametrs 
 * @param env envinronment of run code
*/
void decrem(unit_stack & params, environment & env);

/**
 * increm
 *
 * @param params stack with parametrs 
 * @param env envinronment of run code
*/
void increm(unit_stack & params, environment & env);

// factorial function 
double factorial(double n); 

/**
 * Processing math operations
 * 
 * @param args param`s of operation
 * @param exp name of operation 
 * @param prior priority of operation 
 * 
*/
unit opr_proc(unit_stack & args, std::string exp, int prior);

/**
 * 
 * @param node node with function informations 
 * @param param stack with parametrs 
 * @param env envinronment of run code
 * 
*/
void call_func(unit & node, unit_stack & params,  environment & env);

struct _ctrl{
    _ctrl(environment _env, _type _t){
        env = _env;
        type = _t;
    } 
    ~_ctrl(){};
    environment env;
    _type type;
};

static std::map<_type, key_func> key_words_iterpr{
    {_type::_varInit, var_init },
    {_type::_functionInit, func_init },
    {_type::_if, if_iterpr },
    {_type::_for, for_interpt },
    {_type::_while, while_interpt }
};

static std::map<std::string, simple_func> simple_funcs{
    {"++",[](unit a){return unit(a.increment());}},
    {"--",[](unit a){return unit(a.decrement());}},
    {"nvar",[](unit a){return unit(-a.to_double());}},
    {"sin",[](unit a){return unit(sin(a.to_double()));}},
    {"cos",[](unit a){return unit(cos(a.to_double()));}},
    {"tg",[](unit a){return unit(tan(a.to_double()));}},
    {"ctg",[](unit a){return unit((1/tan(a.to_double())));}},
    {"arcsin",[](unit a){return unit(asin(a.to_double()));}},
    {"arccos",[](unit a){return unit(acos(a.to_double()));}},
    {"arctg",[](unit a){return unit(atan(a.to_double()));}},
    {"arcctg",[](unit a){return unit((atan(-a.to_double())+M_PI_2));}},
    {"abs",[](unit a){return unit(abs(a.to_double()));}},
    {"ln",[](unit a){return unit(log(a.to_double()));}},
    {"deg",[](unit a){return unit((a.to_double()*M_PI/180));}},
    {"sqrt",[](unit a){return unit(pow(a.to_double(),(1/2)));}},
    {"!",[](unit a){return unit((long double)factorial(a.to_double()));}}
};

static std::map<std::string, binary_func> binary_funcs{
    {"==",[](unit a,unit b){return unit(a==b);}},
    {"!=",[](unit a,unit b){return unit(a!=b);}},
    {">",[](unit a,unit b){return unit(a>b);}},
    {"<",[](unit a,unit b){return unit(a<b);}},
    {">=",[](unit a,unit b){return unit(a>=b);}},
    {"<=",[](unit a,unit b){return unit(a<=b);}},
    {"&&",[](unit a,unit b){return unit(a&&b);}},
    {"||",[](unit a,unit b){return unit(a||b);}},
    {"+",[](unit a,unit b){return a+b;}},
    {"-",[](unit a,unit b){return a-b;}},
    {"/",[](unit a,unit b){return a/b;}},
    {"*",[](unit a,unit b){return a*b;}},
    {"%",[](unit a,unit b){return a%b;}},
    {"^",[](unit a,unit b){return unit(pow(a.to_double(),b.to_double()));}},
    {"log",[](unit a,unit b){return unit((log(a.to_double())/log(b.to_double())));}},
    {"round",[](unit a,unit b){return unit(round(a.to_double()*pow(10,b.to_double()))/pow(10,b.to_double()));}}
};

static std::map<std::string, memF> mem_func{
    {"[]", set_in_env},
    {"=", assign },
    {"++", increm },
    {"--", decrem },
    {"print", [](std::stack<unit> & params, environment & env){
        unit_vector _params = set_vars(params,params.size());
        for(size_t _par = 0; _par < _params.size(); _par++){
            if(value(_params[_par]).type == _type::_list){
                std::cout << env.get(value(_params[_par]).name);
               // env.get(value(_params[_par]).name).print();
                continue;
            }
            std::cout << value(_params[_par]);
        }
        std::cout << std::endl;
    }},
    {"size", [](std::stack<unit> & params, environment & env){
        unit_vector _params = set_vars(params,1);
        params.push(unit((long double)value(_params[0]).size()));
    }},
    {"resize", [](std::stack<unit> & params, environment & env){
        unit_vector _params = set_vars(params,2);
        value(_params[0]).resize(value(_params[1]).to_int());
        //params.push(unit((double)value(_params[0]).resize()));
    }},
    {"run", [](std::stack<unit> & params, environment & env){
        unit_vector _params = set_vars(params,1);
        if(_params[0].type != _type::_string){
            throw error(_params[0], "error type!");
        }
        unit_vector _subcode = lex_file(_params[0].to_string(), env);
        _subcode = parse(_subcode,env);
        eval(_subcode, env);
    }},
    {"system", [](std::stack<unit> & params, environment & env){
        unit_vector _params = set_vars(params,1);
        if(_params[0].type != _type::_string){
            throw error(_params[0], "error type!");
        }
        std::system(_params[0].to_string().c_str());
    }},
    {"exit", [](std::stack<unit> & params, environment & env){
        exit(0);
    }},
    {"read",[](std::stack<unit> & params, environment & env){
        unit_vector _params = set_vars(params,params.size());
        for(size_t i = 0; i < _params.size(); i++){
            unit _temp;
            std::cin >> _temp;
            value(_params[i]).assign(_temp);
        }
    }},
    {"clear", [](std::stack<unit> & params, environment & env){
        std::cout << "\x1B[2J\x1B[H";
    }},
    {"+=",[](std::stack<unit> & params, environment & env){
        unit_vector _params = set_vars(params,2);
        value(_params[0]) += value(_params[1]);
    }},
    {"-=",[](std::stack<unit> & params, environment & env){
        unit_vector _params = set_vars(params,2);
        value(_params[0]) -= value(_params[1]);
    }},    
    {"/=",[](std::stack<unit> & params, environment & env){
        unit_vector _params = set_vars(params,2);
        value(_params[0]) /= value(_params[1]);
    }},    
    {"*=",[](std::stack<unit> & params, environment & env){
        unit_vector _params = set_vars(params,2);
        value(_params[0]) *= value(_params[1]);
    }},
    {"%=",[](std::stack<unit> & params, environment & env){
        unit_vector _params = set_vars(params,2);
        value(_params[0]) %= value(_params[1]);
    }},
    {"^=",[](std::stack<unit> & params, environment & env){
        unit_vector _params = set_vars(params,2);
        value(_params[0]) = binary_funcs["^"](value(_params[0]),value(_params[1]));
    }},
};

#endif