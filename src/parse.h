#ifndef __PARSER__H__
#define __PARSER__H__

#include "unit.h"

typedef unit(*parce_key)(unit &, environment &, size_t &);

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




#endif