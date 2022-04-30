#ifndef __PARSER__H__
#define __PARSER__H__

#include "unit.h"

typedef unit(*parce_key)(unit &, environment &);

/**
 *  Parce code tokens 
 *  use "shunting yard algorithm"
 * 
 * @param _tokens vector of code tokens
 * @param env envinronment of run code
 *
 * @return vector of code tokens, in order of execution 
*/
_units parse(_units & _tokens,environment & env);

/**
 *  Parce inti of variables and lists
 * 
 * @param _tokens vector of code tokens
 * @param env envinronment of run code
 * @param positions currient index of _tokens
 * 
 * @return code unit with initializable variables
*/
unit parce_var_init(_units & _tokens,environment & env, int & position);

#endif