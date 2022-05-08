#ifndef __PARSER__H__
#define __PARSER__H__

#include <map>
#include <algorithm>

#include "unit.h"

typedef bool(*cond_func)(unit, std::stack<unit>&);

typedef unit(*parse_key)(_units &, environment &, size_t &);

/**
 *  Parse code tokens 
 *  use "shunting yard algorithm"
 * 
 * @param _tokens vector of code tokens
 * @param env envinronment of run code
 *
 * @return vector of code tokens, in order of execution 
*/
_units parse(_units & _tokens,environment & env);


/**
 *  Find in vector close brt from this index 
 *  
 *  @param _tokens vector wint code units
 *  @param position currient position in _tokens
 * 
 *  @return index in _tokens of close brt )}]
*/
size_t find_close_brt(_units & _tokens, size_t position);


/**
 *  Сopy objects from the vector
 *  
 *  @param _begin start position 
 *  @param _end   stop position 
 * 
 *  @return vector with elements in a certain range 
*/
_units copy_from(_units & _tokens, size_t _begin, size_t _end);

/**
 *  Сut objects from the vector
 *  
 *  @param _begin start position 
 *  @param _end   stop position 
 * 
 *  @return vector with elements in a certain range 
*/
_units cut_from(_units & _tokens, size_t & _begin, size_t _end);

/**
 *  Parse inti of variables and lists
 * 
 * @param _tokens vector of code tokens
 * @param env envinronment of run code
 * @param position currient index of _tokens
 * 
 * @return code unit with initializable variables
*/
unit parse_var_init(_units & _tokens,environment & env, size_t & position);

/**
 *  Parse list init 
 *  
 *  @param _tokens vector with list childs 
 * 
 *  @return vector of list initialization
*/
_units parse_list(_units _tokens);

/**
 *  Parse inti of functions
 * 
 * @param _tokens vector of code tokens
 * @param env envinronment of run code
 * @param position currient index of _tokens
 * 
 * @return code unit with initializable function
*/
unit parse_func_init(_units & _tokens,environment & env, size_t & position);


/**
 *  Parse condition and body of 
 *      key words: if, for, while and body of functions  
 * 
 * @param _tokens vector of code tokens
 * @param env envinronment of run code
 * @param position currient index of _tokens
 * 
 * @return code unit with token
*/
unit parse_token_condition(_units & _tokens,environment & env, size_t & position);

/**
 *  Parse if and else  
 * 
 * @param _tokens vector of code tokens
 * @param env envinronment of run code
 * @param position currient index of _tokens
 * 
 * @return code unit with token
*/
unit parse_if(_units & _tokens,environment & env, size_t & position);

/**
 *  Parse loop while and for
 * 
 * @param _tokens vector of code tokens
 * @param env envinronment of run code
 * @param position currient index of _tokens
 * 
 * @return code unit with token
*/
unit parse_loop(_units & _tokens,environment & env, size_t & position);


/**
 *  Push token in stack to vector, if result of cond_func == true
 * 
 * @param _tokens vector of code tokens
 * @param opr stack with operations
 * @param token currient token
 * @param func func with condition
 * 
*/
void push_token_if(_units & _tokens, std::stack<unit> & opr, unit token, cond_func func);

/**
 *  Push tokens in stack to vector
 *  
 *  @param _tokens vector of code tokens
 *  @param opr stack with operations
 *
*/
void push_stack_to_output(_units & _tokens, std::stack<unit> & opr);

/**
 * Parse close bracket
 * 
 *  @param _tokens vector of code tokens
 *  @param opr stack with operations
 *  @param token currient token
 *  
*/
void parse_close_brt(_units & _tokens,std::stack<unit> & opr, unit token);


static std::map<_type, parse_key> _key_words_parse {
    {_type::_varInit, parse_var_init},
    {_type::_functionInit, parse_func_init},
    {_type::_if, parse_if},
    {_type::_while, parse_loop},
    {_type::_for, parse_loop}
};


#endif