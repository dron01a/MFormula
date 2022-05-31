#include <iostream>
#include <iomanip>
#include <fstream>
#include "string.h"


#include "lexalz.h"
#include "parse.h"


/**
    @param file name of file with source code
    @return file stream 
*/
std::ifstream open_file(std::string file);

/**
    @param file name of file with source code
    @return vector with code unit
*/
unit_vector lex_file(std::string file);

int main(int argc, char *argv[]){
    if(argc <= 1 ){
        std::cout << "not valid args";
    }
    if(strcmp(argv[1],"-f") == 0){
        environment env;
        try{
            unit_vector _code = lex_file(argv[2]);
            _code = parse(_code, env);
        }
        catch(error & _error){
            std::ifstream in = open_file(argv[2]); // file with code
            std::string _buf; // temp string 
            std::string _error_point;
            size_t _str = 0;
            while(std::getline(in,_buf)){
                if(_str == _error._unit._str){
                    break;
                }
                _str++;
            }
            _error_point.resize(_buf.size());
            _error_point.insert(0, _error_point.size(),'-');
            _error_point.insert(_error._unit._col, "^");
            printf("line:%zu col:%zu\terror: %s\n\n", _error._unit._str,_error._unit._col, _error.message.c_str());
            printf("%s\n%s\n",_buf.c_str(),_error_point.c_str());
        }
    }
    return 0;
}

unit_vector lex_file(std::string name){
    unit_vector _result; // result of function
    unit_vector _temp;   // temp vector
    std::ifstream in = open_file(name); // file with code     
    std::string _buf; // temp string 
    int _cur_string = 0;
    while(std::getline(in,_buf)){
        _temp = lex(_buf, _cur_string);  // get units of string
        if(_temp.size() != 0 && _temp[0].type == _type::_include){
            _temp = lex_file(_temp[1].name);
        } 
        for(size_t i = 0; i < _temp.size(); i++){
            _result.push_back(_temp[i]); // load to result
        }
        _cur_string++;
    }
    in.close();
    return _result;
}

std::ifstream open_file(std::string name){
    std::ifstream in(name,std::ios::binary); // file with code
    if(in.bad()){
        std::cout << "not found file " << name << std::endl;
        throw;
    }
    return in;
}