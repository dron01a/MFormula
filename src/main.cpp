#include <iostream>
#include <iomanip>

#include "string.h"

#include "lexalz.h"
#include "parse.h"
#include "processing.h"
#include "fileio.h"

void error_proc(error & _err, std::string source, std::string mode);

int main(int argc, char *argv[]){
    if(argc < 1 ){
        std::cout << "not valid args";
    }
    environment env;
    unit_vector _code;
    if(argc == 1){
        while(true){
            std::string source; // string with command 
            try{
                std::cout << "\nmf > ";
                getline(std::cin,source);
                _code = lex(source += ";",0);
                _code = parse(_code, env);
                eval(_code, env);
                if(_code.size() != 0 ){
                    if(_code[0].type == _type::_num || _code[0].type == _type::_string || _code[0].type == _type::_var || _code[0].type == _type::_list){
                        _code[0].print();
                    }
                }
            }
            catch(error _error){
                error_proc(_error, source, "-s");
            }
            catch(const char * _error){
                std::cout << _error << std::endl;
            }
        }
    }
    else {
        try{
            if(strcmp(argv[1],"-s") == 0){
                _code = lex(std::string("print("+std::string(argv[2])+")"), 0);    // lex data
            }
            if(strcmp(argv[1],"-f") == 0){
                _code = lex_file(argv[2]);
            }
            _code = parse(_code, env);  // parce data
            eval(_code, env);           // eval
        }
        catch(error _error){
            error_proc(_error, argv[2], argv[1]);
        }
        catch(const char * _error){
            std::cout << _error << std::endl;
        }
    }
    return 0;
}

void error_proc(error & _err, std::string source, std::string mode){
    std::string _error_point;
    std::string _buf; // temp string 
    if(mode == "-f"){
        std::ifstream in = open_file(source); // file with code
        std::string _error_point;
        size_t _str = 0;
        while(std::getline(in,_buf)){
            if(_str == _err._unit._str){
                break;
            }
            _str++;
        }
    }
    if(mode == "-s"){
        _buf = source;
    }
    _error_point.insert(0, _buf.size(),'-');
    _error_point.insert(_err._unit._col, "^");
    printf("line:%zu col:%zu\nerror: %s\n\n", _err._unit._str,_err._unit._col, _err.message.c_str());
    printf("%s\n%s\n",_buf.c_str(),_error_point.c_str());
}
