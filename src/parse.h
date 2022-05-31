#ifndef __PARSER__H__
#define __PARSER__H__

#include <map>
#include <algorithm>

#include "unit.h"

typedef bool(*cond_func)(unit, unit_stack &);

typedef unit(*parse_key)(unit_vector &, environment &, size_t &);

/**
 *  Parse code tokens 
 *  use "shunting yard algorithm"
 * 
 * @param _tokens vector of code tokens
 * @param env envinronment of run code
 *
 * @return vector of code tokens, in order of execution 
*/
unit_vector parse(unit_vector & _tokens,environment & env);


/**
 *  Find in vector close brt from this index 
 *  
 *  @param _tokens vector wint code units
 *  @param position currient position in _tokens
 * 
 *  @return index in _tokens of close brt )}]
*/
size_t find_close_brt(unit_vector & _tokens, size_t position);


/**
 *  Сopy objects from the vector
 *  
 *  @param _begin start position 
 *  @param _end   stop position 
 * 
 *  @return vector with elements in a certain range 
*/
unit_vector copy_from(unit_vector & _tokens, size_t _begin, size_t _end);

/**
 *  Сut objects from the vector
 *  
 *  @param _begin start position 
 *  @param _end   stop position 
 * 
 *  @return vector with elements in a certain range 
*/
unit_vector cut_from(unit_vector & _tokens, size_t & _begin, size_t _end);

/**
 *  Parse inti of variables and lists
 * 
 * @param _tokens vector of code tokens
 * @param env envinronment of run code
 * @param position currient index of _tokens
 * 
 * @return code unit with initializable variables
*/
unit parse_var_init(unit_vector & _tokens,environment & env, size_t & position);

/**
 *  Parse list init 
 *  
 *  @param _tokens vector with list childs 
 * 
 *  @return vector of list initialization
*/
unit_vector parse_list(unit_vector _tokens);

/**
 *  Parse inti of functions
 * 
 * @param _tokens vector of code tokens
 * @param env envinronment of run code
 * @param position currient index of _tokens
 * 
 * @return code unit with initializable function
*/
unit parse_func_init(unit_vector & _tokens,environment & env, size_t & position);


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
unit parse_token_condition(unit_vector & _tokens,environment & env, size_t & position);

/**
 *  Parse if and else  
 * 
 * @param _tokens vector of code tokens
 * @param env envinronment of run code
 * @param position currient index of _tokens
 * 
 * @return code unit with token
*/
unit parse_if(unit_vector & _tokens,environment & env, size_t & position);

/**
 *  Parse loop while and for
 * 
 * @param _tokens vector of code tokens
 * @param env envinronment of run code
 * @param position currient index of _tokens
 * 
 * @return code unit with token
*/
unit parse_loop(unit_vector & _tokens,environment & env, size_t & position);


/**
 *  Push token in stack to vector, if result of cond_func == true
 * 
 * @param _tokens vector of code tokens
 * @param opr stack with operations
 * @param token currient token
 * @param func func with condition
 * 
*/
void push_token_if(unit_vector & _tokens, unit_stack & opr, unit token, cond_func func);

/**
 *  Push tokens in stack to vector
 *  
 *  @param _tokens vector of code tokens
 *  @param opr stack with operations
 *
*/
void push_stack_to_output(unit_vector & _tokens, unit_stack & opr);

/**
 * Parse close bracket
 * 
 *  @param _tokens vector of code tokens
 *  @param opr stack with operations
 *  @param token currient token
 *  
*/
void parse_close_brt(unit_vector & _tokens,unit_stack & opr, unit token);


static std::map<_type, parse_key> _key_words_parse {
    {_type::_varInit, parse_var_init},
    {_type::_functionInit, parse_func_init},
    {_type::_if, parse_if},
    {_type::_while, parse_loop},
    {_type::_for, parse_loop}
};


#endif