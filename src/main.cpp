#include <iostream>
#include <string>
#include <string.h>
#include <vector>

#define NPOS std::string::npos

enum exmpElements{
    _num,    // numbers -> 0-9  
    _brt,    // brackets -> () [] {} 
    _opr,    // operations -> +-*/^%
    _func,   // functions -> abs sin cos tg ctg 
    _special // special chars -> e pi 
};

typedef std::pair<exmpElements, char> unit;

typedef std::vector<unit> exmpUnits;

std::string example = "sssin(39)^78"; // for example 

// return type of character 
exmpElements getCharType(std::string source, size_t position);

// parce example
exmpUnits parceExmpl(std::string source);

// throw unknown char
void unknown(char c);

int main(){
    exmpUnits test = parceExmpl(example);
    return 0;
}

exmpElements getCharType(std::string source, size_t position){
    std::string brackets = "[]{}()";
    std::string operators = "+-*/^%";
    std::string special = "!~E";         
    std::string numbers = "0123456789.,";
    if(brackets.find(source[position]) != NPOS){
        return _brt;
    } 
    else if(operators.find(source[position]) != NPOS){
        return _opr;
    }
    else if(special.find(source[position]) != NPOS){
        return _special;
    }
    else if(numbers.find(source[position]) != NPOS){
        return _num;
    }
    else{
        switch (source[position]){
        case 'p' :
            if(source.find("pi", position) != position){
                unknown(source[position]);
            }
            return _special;
            break;
        case 'a':
            if(source.find("abs", position) != position){
                unknown(source[position]);
            }
            return _func;
            break;
        case 's':
            if(source.find("sin", position) != position){
                unknown(source[position]);
            }
            return _func;
            break;
        case 'c':
            if(source.find("cos", position) != position || source.find("ctg", position) != position){
                unknown(source[position]);
            }
            return _func;
            break;
        case 't':
            if(source.find("tg", position) != position){
                unknown(source[position]);
            }
            return _func;
            break;
        default:
            unknown(source[position]);
            break;
        }
    }
}

exmpUnits parceExmpl(std::string source){
    exmpUnits result;
    for(size_t i = 0; i < source.size(); i++){
        try {
            exmpElements type = getCharType(source, i);
            result.push_back(unit(type,source[i]));
            if(source[i]=='a'){
                result.push_back(unit(type,source[i+1]));
                result.push_back(unit(type,source[i+2]));
                i+=2;
            }
            if(source[i]=='s'){
                result.push_back(unit(type,source[i+1]));
                result.push_back(unit(type,source[i+2]));
                i+=2;
            }
            if(source[i]=='c'){
                result.push_back(unit(type,source[i+1]));
                result.push_back(unit(type,source[i+2]));
                i+=2;
            }
            if(source[i]=='t'){
                result.push_back(unit(type,source[i+1]));
                i++;
            }
            if(source[i]=='p'){
                result.push_back(unit(type,source[i+1]));
                i++;
            }
        }  
        catch(std::string mes){
            std::cout << mes.c_str() << source[i] << std::endl;
        }
    }
    return result;
}

void unknown(char c){
    throw std::string("unknown char: "); 
}