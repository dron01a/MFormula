#include "parcer.h"

Parcer::Parcer(_units & units, environment & env){
    unit current;
    std::stack<unit> oprStack;
    for(int count = 0; count < units.size(); count++){
        switch (units[count].type){
        case _type::_varInit:
            
        case _type::_special: 
            parceCloseBrt(oprStack,units[count],'(');
            break;
        case _type::_text:
        case _type::_var:
        case _type::_num:
             _tokens.push_back(units[count]);
            break;
        case _type::_brt:  
            if(units[count].name == "(" || units[count].name == "[" || units[count].name == "{"){
                checkCloseBrt(units,count);
                oprStack.push(units[count]);           
            }
            else{
                parceCloseBrt(oprStack,units[count],units[count].name[0]);
            } 
            break;
        case _type::_opr:
            getUnitsIn(oprStack,units[count],[](unit _unit, std::stack<unit> & oprStack){
                return oprStack.size() && ((oprStack.top().type == _type::_opr && _unit.prior <= oprStack.top().prior) || oprStack.top().type == _type::_func );
            });
            oprStack.push(units[count]);
            break;
        case _type::_coreFunc:
        case _type::_func:
            getUnitsIn(oprStack,units[count], [](unit _unit, std::stack<unit> & oprStack){
                return oprStack.size() && oprStack.top().type == _type::_func;
            });
            oprStack.push(units[count]);
            break;
        }
    }
    getUnitsIn(oprStack,units.back(),[](unit _unit, std::stack<unit> & oprStack){ return oprStack.size() != 0; });
}

_units Parcer::getTokens() { 
    return _tokens; 
}

void Parcer::getUnitsIn(std::stack<unit> & oprStack, unit curUnit, condFunc func){
    if(oprStack.size()){
        while(func(curUnit, oprStack)){
            _tokens.push_back(oprStack.top());
            oprStack.pop();
        }
    }
}

bool Parcer::checkCloseBrt(_units & units, int position){
    std::string brtType;
    int openChars = 0; 
    switch(units[position].name[0]){
        case '(':brtType = ")";break;
        case '[':brtType = "]";break;
        case '{':brtType = "}";break;
    }
    for(int count = position;; count++){
        if(units[count].name == units[position].name){
            openChars++;
        }
        if(units[count].name == brtType){
            break;
        }
    }
    for(int count = position; (openChars != 0) && (count < units.size()) ;count++){
        if(units[count].name == brtType){
            openChars--;
        }
    }
    if(openChars != 0){
        throw std::string("not found closing character \"" + brtType + "\"");
    }
    return true;
}

void Parcer::parceCloseBrt(std::stack<unit> & oprStack, unit curUnit, char _type){
    switch(_type){
        case ')':
            getUnitsIn(oprStack,curUnit,[](unit _unit, std::stack<unit> & oprStack){
                return oprStack.top().name != "(";
            });
            break;
        case ']':
            getUnitsIn(oprStack,curUnit,[](unit _unit, std::stack<unit> & oprStack){
                return oprStack.top().name != "[";
            });
            break;
        case '}':
            getUnitsIn(oprStack,curUnit,[](unit _unit, std::stack<unit> & oprStack){
                return oprStack.top().name != "{";
            });
        break;
    }
    oprStack.pop();
}