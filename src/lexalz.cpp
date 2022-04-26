#include "lexalz.h"

_units lex_file(std::string name){
    _units _result; // result of function
    _units _temp;   // temp vector
    std::ifstream in(name,std::ios::binary); // file with code
    std::string _buf; // temp string 
    int _cur_string = 0;
    while(std::getline(in,_buf)){
        _temp = lex(_buf, _cur_string);  // get units of string 
        for(size_t i = 0; i < _temp.size(); i++){
            _result.push_back(_temp[i]); // load to result
        }
        _cur_string++;
    }
    in.close();
    return _result;
}


_units lex(std::string & source, int _str){
    _units _result; // result of function
    size_t _col = 0;   // cur col 
    int _next_line = source.find('\n',0);
    for(int count = 0; count < source.size(); count++){
        _col = count;
        std::string token; // temp string for string with token
        if(source[count] == '/' && source[count+1] == '/'){
            count = source.find("\n",count+1); // skip comments
        }
        if(std::isspace(source[count])){
            continue; // skip staces
        }
        if(source[count] == '\"'){
            int _curPos = count + 1;
            count = source.find("\"",count+1);
            token = source.substr(_curPos,count - _curPos);
            _result.push_back(unit(_type::_string, token, 0)); // add string unit
            _result.back()._col = _col;
            _result.back()._str = _str;
            continue;
        }
        else if(_delim.find(source[count])==NPOS){
            add_to_token(source,token,count,[](std::string _delim, char _char){
                return _delim.find(_char)==NPOS;
            });
        }
        else{
            add_to_token(source,token,count,[](std::string _delim, char _char){
                return _delim.find(_char)!=NPOS;
            });
        }
        if(token == "-" && _result.back().type == _type::_openBrt){
            token = "nvar";
        }
        _result.push_back(unit(get_type(token), token, get_priority(token)));
        _result.back()._col = _col;
        _result.back()._str = _str;
      //  _col += token.size() + 1;
    }
    return _result;
}

void add_to_token(std::string & _source, std::string & _token, int & _count, compareFunc _func){
    while(_func(_delim, _source[_count]) && _source[_count] != '\0'){
        if(std::isspace(_source[_count])){
            _count++;
            break;
        }
        if(_source[_count] == '.'){
            if(get_type(_token) != _type::_num && get_type(std::string(1,_source[_count+1])) != _type::_num){
//                _count++;
                break;
            }                                  
            if(_token.size() == 0){
                _token+=_source[_count];
                _count++;
                break;
            }
        } 
        _token+=_source[_count];
        _count++;
        if(_source[_count] == ';'){
            break;
        }
        if( get_type(std::string(1,_source[_count])) == _type::_openBrt 
            || get_type(std::string(1,_source[_count])) == _type::_closeBrt
            || get_type(_token) == _type::_openBrt
            || get_type(_token) == _type::_closeBrt ){
            break;
        } 
    }
    _count--;
}

_type get_type(std::string _token){
    if(_token == "true" || _token == "false"){
        return _type::_bool;
    }
    if(_token.size() == 0 && _token == "."){
        return _type::_return;
    }
    if(_token == "if"){
        return _type::_if;
    }
    if(_token == "else"){
        return _type::_else;
    }
    if(_token == "for"){
        return _type::_for;
    }
    if(_token == "while"){
        return _type::_while;
    }
    if(_token == "break"){
        return _type::_break;
    }
    if(_token == "return"){
        return _type::_return;
    }
    if(_token == "continue"){
        return _type::_continue;
    }
    if(_token == "include"){
        return _type::_include;
    }
    if(_token == "."){
        return _type::_opr;
    }
    if(_token.find_first_not_of(".0123456789") == NPOS){
        return _type::_num;
    }
    if(openBrt.find(_token) != NPOS){
        return _type::_openBrt;
    } 
    if(closeBrt.find(_token) != NPOS){
        return _type::_closeBrt;
    } 
    else if(operators.find(_token) != NPOS || _token == "==" || _token == "!=" 
        ||_token == ">=" || _token == "<=" || _token == "&&" || _token == "||"
        || _token == "++" || _token == "--"){
        return _type::_opr;
    }
    else if(_token == ","){
        return _type::_special;
    }
    else if (_token == "var"){
        return _type::_varInit;
    }
    else if (_token == "func"){
        return _type::_functionInit;
    }
    else if (_token == ";" || _token == "\n"){
        return _type::_semicolon;
    }
    else if (_token.find("\"") != NPOS){
        return _type::_string;
    }
    else{
        for(int count = 0; count < functions.size(); count++ ){
            if(_token == functions[count]){
                return _type::_coreFunc;
            }
        }
    }
    return _type::_indentf;
}

int get_priority(std::string & _token){
    if (_token == "==" || _token == "!=" ||_token == ">=" || _token == "<=" || _token == ">"|| _token == "<"){
        return 1;
    }
    if (_token == "&&" || _token == "||"){
        return 0;
    }
    if (_token == "+" || _token == "-"  ) {
        return 2;
    }
	if (_token == "*" || _token == "/" || _token == "%"){ 
        return 3;
    }
	if (_token == "^"|| _token == "nvar" || _token == "++" || _token == "--") {
        return 4;
    }
    if (_token == "!"){
        return 5;
    }
    return 0;
}