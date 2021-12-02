#include "lexer.h"

Lexer::Lexer(std::string source, environment & env){
    this->_source = source;
    std::string token;
    for(int count = 0; count < source.size(); count++){
        token.clear();   
        if(source[count] == '/' && source[count+1] == '/'){
            count = source.find("\n",count+1);
        }
        if(source[count] == '\"'){
            int _curPos = count + 1;
            count = source.find("\"",count+1);
            token = source.substr(_curPos,count - _curPos);
            units.push_back(unit(_type::_text, token, 0));
            //count++;
            continue;
        }
        if(std::isspace(source[count])){
            continue;
        }
        else if(_delim.find(source[count])==NPOS){
            addToToken(token,count,[](std::string _delim, char _char){
                return _delim.find(_char)==NPOS;
            });
        }
        else{
            addToToken(token,count,[](std::string _delim, char _char){
                return _delim.find(_char)!=NPOS;
            });
        }
        if(env.have(token)){
            units.push_back(unit(env.get(token).type, token));
        }
        else{
            units.push_back(unit(getType(token), token, getPriority(token)));
        }
    }
}

_units Lexer::getUnits(){
    return units;
}

_type Lexer::getType(std::string exp){
    if(exp == "true" || exp == "false"){
        return _type::_bool;
    }
    if(exp == "if"){
        return _type::_if;
    }
    if(exp == "else"){
        return _type::_else;
    }
    if(openBrt.find(exp) != NPOS){
        return _type::_openBrt;
    } 
    if(closeBrt.find(exp) != NPOS){
        return _type::_closeBrt;
    } 
    else if(operators.find(exp) != NPOS || exp == "==" || exp == "!=" ||exp == ">=" || exp == "<="){
        return _type::_opr;
    }
    else if(exp == ","){
        return _type::_special;
    }
    else if(exp.find_first_not_of("0123456789.") == NPOS){
        return _type::_num;
    }
    else if (exp == "var"){
        return _type::_varInit;
    }
    else if (exp == "func"){
        return _type::_functionInit;
    }
    else if (exp == ";" || exp == "\n"){
        return _type::_semicolon;
    }
    else if (exp.find("\"") != NPOS){
        return _type::_text;
    }
    else{
        for(int count = 0; count < 12; count++ ){
            if(exp == functions[count]){
                return _type::_coreFunc;
            }
        }
    }
    return _type::_indentf;
}

void Lexer::addToToken(std::string & _token, int & _count, compareFunc _func){
    while(_func(_delim, _source[_count]) && _source[_count] != '\0'){
        if(std::isspace(_source[_count])){
            _count++;
            break;
        } 
        _token+=_source[_count];
        _count++;
        if( getType(std::string(1,_source[_count])) == _type::_openBrt 
            || getType(std::string(1,_source[_count])) == _type::_closeBrt
            || getType(_token) == _type::_openBrt
            || getType(_token) == _type::_closeBrt ){
            break;
        } 
    }
    _count--;
}


int Lexer::getPriority(std::string exp){
    if (exp == "==" || exp == "!=" ||exp == ">=" || exp == "<="){
        return 1;
    }
    if (exp == "+" || exp == "-" ) {
        return 2;
    }
	if (exp == "*" || exp == "/" || exp == "%"){ 
        return 3;
    }
	if (exp == "^") {
        return 4;
    }
    if (exp == "!"){
        return 5;
    }
    return 0;
}