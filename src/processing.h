#ifndef __PROC__H__
#define __PROC__H__

#include "unit.h"
#include "parse.h"

typedef unit(*simpleF)(unit);
typedef unit(*binaryF)(unit, unit);
typedef void(*memF)(unit_stack &, environment & );
typedef void(*keyFunc)(unit &, environment & );

/**
 * processes parts of the code
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
 * @param node needed node
 * @param env envinronment of run code
*/
void eval_list(unit & node, environment &env); 


#endif