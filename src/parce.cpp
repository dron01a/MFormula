#include "parce.h"

exmpUnits parce(std::string source){
    exmpUnits result;
    std::string token;
    std::string delim = "[]()+-*/^%,";
    for(int count = 0; count < source.size(); count++){
        token.clear();
        if(std::count(delim.begin(),delim.end(),source[count]) > 0){
            if(std::isspace(source[count])){
                continue;
            }
            token = source[count];
        }
        else{
            while(std::count(delim.begin(),delim.end(),source[count]) == 0){
                if(source[count] == '\0'){
                    break;
                }
                token+=source[count];
                count++;
            }
            count--;
        }
        result.push_back(unit(getType(token), token, getPriority(token)));
    }
    return result;
}

exmpElements getType(std::string exp){
    std::string brackets = "[]()";
    std::string operators = "+-*/^%";
    std::string special = "e,";         
    std::string numbers = "0123456789.";
    std::string functions[10]{ 
        "abs", "arc", "cos", "sin", "tg", "ln", "ctg", "sqrt", "!", "log"
    };
    if(brackets.find(exp) != NPOS){
        return _brt;
    } 
    else if(operators.find(exp) != NPOS){
        return _opr;
    }
    else if(special.find(exp) != NPOS || exp == "pi"){
        return _special;
    }
    else if(exp.find_first_not_of("0123456789.") == NPOS){
        return _num;
    }
    else{
        for(int count = 0; count < 10; count++ ){
            if(exp == functions[count]){
                return _func;
            }
        }
    }
    unknown(exp);
}

void unknown(std::string c){
    throw std::string("unknown: ") += c; 
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
        unit current = _units[count];
        switch (current.type){
        case _special:
        case _num:
            result.push_back(current);
            break;
        case _brt:  
            if(current.exp == "(" || current.exp == "["){
                findCloseBrt(_units,count);
                oprStack.push(current);
            }
            if(current.exp == ")"){
                getUnitsIn("(",result,oprStack);
            }
            if(current.exp == "]"){
                getUnitsIn("[",result,oprStack);
            }               
            break;
        case _opr:
            getUnitsIn(result, oprStack,current,[](unit _unit, std::stack<unit> & oprStack){
                return oprStack.size() &&  ((oprStack.top().type == _opr && _unit.prior <= oprStack.top().prior) || oprStack.top().type == _func);
            });
            oprStack.push(current);
            break;
        case _func:
            getUnitsIn(result, oprStack,current, [](unit _unit, std::stack<unit> & oprStack){
                return oprStack.size() && oprStack.top().type == _func;
            });
            oprStack.push(current);
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

void getUnitsIn(exmpUnits & _units, std::stack<unit> & oprStack, unit curUnit, condFunc func){
    if(oprStack.size()){
        bool condition = func(curUnit, oprStack);
        while(condition){
            _units.push_back(oprStack.top());
            oprStack.pop();
            condition = func(curUnit, oprStack);
        }
    }
}