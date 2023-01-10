#include "fileio.h"

unit_vector lex_file(std::string name, environment & env){
    unit_vector _result; // result of function
    unit_vector _temp;   // temp vector
    std::ifstream in = open_file(name); // file with code     
    std::string _buf; // temp string 
    int _cur_string = 0;
    while(std::getline(in,_buf)){
        _temp = lex(_buf, _cur_string);  // get units of string
        if(_temp.size() != 0 && _temp[0].type == _type::_include){
            _temp = lex_file(_temp[1].name, env);
            _temp = parse(_temp, env);
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
        throw "not found file " + name; 
    }
    return in;
}
