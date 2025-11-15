#ifndef __MAIN__H__
#define __MAIN__H__

#define VERSION "1.6"

#include <iostream>
#include <iomanip>
#include "lexer.h"
#include "script.h"

#define _ERROR(source, mode)\
	catch(mf::error _error){ error_proc(_error, source, mode);}\
	catch(const char *_error){std::cout << _error << std::endl;}\
	catch(std::string _error){std::cout << _error << std::endl;}

void error_proc(mf::error & _err, std::string source, std::string mode);

// ÓÚÍ˚Ú¸ Ù‡ÈÎ Ò ÍÓ‰ÓÏ
//std::ifstream open_file(std::string file);

// „Î‡‚Ì‡ˇ ÙÛÌÍˆËË ÔÓ„‡ÏÏ˚
int main(int argc, char *argv[]);

#endif 

/*
                  __________        __________
                  \--------/ <\  /> \--------/
                   \-------\__/  \__/-------/
                     \------\      /------/
                             /\  /\
							"' \/ '"
                      |»Ãœ≈–¿“Œ– «¿Ÿ»“»“|  
                      |—À¿¬¿ ¡Œ√” Ã¿ÿ»Õ≈|
*/
