#include "parce.h"

exmpElements getCharType(std::string source, size_t position){
    std::string brackets = "[]()";
    std::string operators = "+-*/^%";
    std::string special = "!~Ee";         
    std::string numbers = "0123456789.";
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

exmpChars parceExmpl(std::string source){
    exmpChars result;
    for(size_t i = 0; i < source.size(); i++){
        exmpElements type = getCharType(source, i);
        result.push_back(std::make_pair(type,source[i]));
        if(type == _func || type == _special){
            if(source[i]=='a'){
                result.push_back(std::make_pair(type,source[i+1]));
                result.push_back(std::make_pair(type,source[i+2]));
                i+=2;
            }
            if(source[i]=='s'){
                result.push_back(std::make_pair(type,source[i+1]));
                result.push_back(std::make_pair(type,source[i+2]));
                i+=2;
            }
            if(source[i]=='c'){
                result.push_back(std::make_pair(type,source[i+1]));
                result.push_back(std::make_pair(type,source[i+2]));
                i+=2;
            }
            if(source[i]=='t'){
                result.push_back(std::make_pair(type,source[i+1]));
                i++;
            }
            if(source[i]=='p'){
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
    if ((elem != curElem) || (elem == _brt && curElem == _brt)){
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
    for(result = position;;result++){
        if(units[result].exp == brtType){
            openChars--;
        }
        if(openChars == 0){
            break;
        }
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
    return 0;
}

void creatrePostfix(exmpUnits & _units, exmpUnits & _sortUnits){
    std::stack<unit> oprStack;
    for(int count = 0; count < _units.size(); count++){
        switch (_units[count].type){
        case _special:
        case _num:
            _sortUnits.push_back(_units[count]);
            break;
        case _brt:  
            if(_units[count].exp == "(" || _units[count].exp == "["){
                oprStack.push(_units[count]);
            }
            if(_units[count].exp == "]" || _units[count].exp == ")"){
                int stopPos = findCloseBrt(_units,count);
                while(oprStack.top().exp != "("){
                    _sortUnits.push_back(oprStack.top());
                    oprStack.pop();
                }
                oprStack.pop();
            }           
            break;
        case _opr:
            if(oprStack.size() != 0){
                while(oprStack.size() != 0 && (oprStack.top().type == _opr && _units[count].prior <= oprStack.top().type)){
                    _sortUnits.push_back(oprStack.top());
                    oprStack.pop();
                }
            }
            oprStack.push(_units[count]);
            break;
        case _func:
            while (oprStack.size() && oprStack.top().type == _func){
                _sortUnits.push_back(oprStack.top());
                oprStack.pop();
            }
            oprStack.push(_units[count]);
        break;
        default:
            break;
        }
    }
    while(oprStack.size()){
        _sortUnits.push_back(oprStack.top());
        oprStack.pop();
    }
}

void unknown(char c){
    throw std::string("unknown char: ") += c; 
}