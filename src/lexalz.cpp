#include "lexalz.h"

_units lex(std::string & source, environment & env){
    _units _result; // result of function
    for(int count = 0; count < source.size(); count++){
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
            continue;
        }
        else if(_delim.find(source[count])==NPOS){
            add_to_token(token,count,[](std::string _delim, char _char){
                return _delim.find(_char)==NPOS;
            });
        }
        else{
            add_to_token(token,count,[](std::string _delim, char _char){
                return _delim.find(_char)!=NPOS;
            });
        }
        if(env.have(token)){
            _result.push_back(unit(env.get(token).type, token));
        }
        else{
            if(token == "-" && _result.back().type == _type::_openBrt){
                token = "nvar";
            }
            _result.push_back(unit(get_type(token, env), token, get_priority(token)));
        }
    }
    return _result;
}