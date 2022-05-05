#ifndef __PARSER__H__
#define __PARSER__H__

#include <map>

#include "unit.h"


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
 *  Parse loop while
 * 
 * @param _tokens vector of code tokens
 * @param env envinronment of run code
 * @param position currient index of _tokens
 * 
 * @return code unit with token
*/
unit parse_while(_units & _tokens,environment & env, size_t & position);

typedef unit(*parse_key)(_units &, environment &, size_t &);

static std::map<_type, parse_key> _key_words_parse {
    {_type::_varInit, parse_var_init},
    {_type::_functionInit, parse_func_init},
    {_type::_if, parse_if},
    {_type::_while, parse_while}

    
};


#endif