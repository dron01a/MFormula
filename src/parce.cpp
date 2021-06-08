#include "parce.h"

exmpElements getCharType(std::string source, size_t & position){
    std::string brackets = "[]()";
    std::string operators = "+-*/^%";
    std::string special = "e,";         
    std::string numbers = "0123456789.";
    std::string functions[10]{ 
        "abs", "arc", "cos", "sin", "tg", "ln", "ctg", "sqrt", "!", "log"
    };
    while(std::isspace(source[position])){
        position++;
    }
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
        if(source.find("pi", position) == position){
            return _special;
        }
        for(int count = 0; count < 10; count++ ){
            if(source.find(functions[count], position) == position){
                return _func;
            }
        }
    }
    unknown(source[position]);
}

exmpChars parceExmpl(std::string source){
    exmpChars result;
    for(size_t i = 0; i < source.size(); i++){
        exmpElements type = getCharType(source, i);
        result.push_back(std::make_pair(type,source[i]));
        if(type == _func || type == _special){
            switch (source[i]){
            case 's':
                if(source[i+1] == 'q'){
                    result.push_back(std::make_pair(type,source[i+1]));
                    i++;
                }
            case 'a':
            case 'c':
                result.push_back(std::make_pair(type,source[i+1]));
                result.push_back(std::make_pair(type,source[i+2]));
                i+=2;
                break;
            case 't':
            case 'p':
            case 'l':
                if(source[i+1] == 'o'){
                    result.push_back(std::make_pair(type,source[i+1]));
                    i++;
                }
                result.push_back(std::make_pair(type,source[i+1]));
                i++;
            }
        }  
    }
    return result;
}

exmpUnits parceChars(exmpChars & _chars){
    exmpUnits result;
    int position = 0;
    result.push_back(unit(_chars[0].first,""));
    distChars(result,_chars[0].first,_chars[0].first,_chars[0].second,position);
    for(int count = 1; count < _chars.size(); count++){
        switch (_chars[count].first){
        case _func:
            distChars(result,_chars[count-1].first,_func,_chars[count].second,position);
            break;
        case _num:
            distChars(result,_chars[count-1].first,_num,_chars[count].second,position);
            break;
        case _brt:
            distChars(result,_chars[count-1].first,_brt,_chars[count].second,position);
            break;
        case _opr:
            distChars(result,_chars[count-1].first,_opr,_chars[count].second,position);
            break;
        case _special:
            distChars(result,_chars[count-1].first,_special,_chars[count].second,position);
            break;
        }
    }
    return result;
}

void distChars(exmpUnits & _units,exmpElements elem, exmpElements curElem, char curChar, int & position){
    if ((elem != curElem) || (elem == _brt && curElem == _brt) || (elem == _opr && curElem == _opr)){
        position++;
        _units.push_back(unit(curElem,""));
    }
    _units[position].exp += curChar;
    _units[position].prior = getPriority(_units[position].exp);
}

int findCloseBrt(exmpUnits & units, int position){
    std::string brtType;
    int openChars = 0; 
    int result = 0;
    if(units[position].exp == "("){
        brtType = ")";
    }
    if(units[position].exp == "["){
        brtType = "]";
    }
    for(int count = position;; count++){
        if(units[count].exp == units[position].exp){
            openChars++;
        }
        if(units[count].exp == brtType){
            break;
        }
    }
    for(result = position;result < units.size();result++){
        if(units[result].exp == brtType){
            openChars--;
        }
        if(openChars == 0){
            break;
        }
    }
    if(openChars != 0){
        throw std::string("not found closing character \"" + brtType + "\"");
    }
    return result;
}

int getPriority(std::string exp){
    if (exp == "+" || exp == "-" ) {
        return 1;
    }
	if (exp == "*" || exp == "/" || exp == "%"){ 
        return 2;
    }
	if (exp == "^") {
        return 3;
    }
    if (exp == "!"){
        return 4;
    }
    return 0;
}

exmpUnits creatrePostfix(exmpUnits & _units){
    exmpUnits result;
    std::stack<unit> oprStack;
    for(int count = 0; count < _units.size(); count++){
        switch (_units[count].type){
        case _special:
        case _num:
            result.push_back(_units[count]);
            break;
        case _brt:  
            if(_units[count].exp == "(" || _units[count].exp == "["){
                findCloseBrt(_units,count);
                oprStack.push(_units[count]);
            }
            if(_units[count].exp == ")"){
                getUnitsIn("(",result,oprStack);
            }
            if(_units[count].exp == "]"){
                getUnitsIn("[",result,oprStack);
            }               
            break;
        case _opr:
            if(oprStack.size()){
                while(oprStack.size() && ((oprStack.top().type == _opr && _units[count].prior <= oprStack.top().prior) || oprStack.top().type == _func)){
                    result.push_back(oprStack.top());
                    oprStack.pop();
                }
            }
        case _func:
            while (oprStack.size() && oprStack.top().type == _func){
                result.push_back(oprStack.top());
                oprStack.pop();
            }
            oprStack.push(_units[count]);
            break;
        }
    }
    while(oprStack.size()){
        result.push_back(oprStack.top());
        oprStack.pop();
    }
    return result;
}

void getUnitsIn(std::string obj,exmpUnits & _units, std::stack<unit> & oprStack){
    while(oprStack.top().exp != obj){
        _units.push_back(oprStack.top());
        oprStack.pop();
    }       
    oprStack.pop();
}

void unknown(char c){
    throw std::string("unknown char: ") += c; 
}