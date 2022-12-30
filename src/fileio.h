#ifndef __FILEIO__
#define __FILEIO__

#include <fstream>
#include "lexalz.h"
#include "parse.h"

/**
    @param file name of file with source code
    @return file stream 
*/
std::ifstream open_file(std::string file);

/**
    @param file name of file with source code
    @param env envinronment of run code
    @return vector with code unit
*/
unit_vector lex_file(std::string file, environment & env);

#endif