#include "parcer.h"

Parcer::Parcer(_units & units, environment & env){
    std::stack<unit> oprStack;
    for(int count = 0; count < units.size(); count++){
        switch (units[count].type){
        case _type::_varInit:
            units[count]._childs.push_back(units[count+1]);
            _tokens.push_back(units[count]);
            parceVarInit(units,env,count);
            break;
        case _type::_text:
        case _type::_var:
        case _type::_num:
            _tokens.push_back(units[count]);
            break;
        case _type::_special: 
            getUnitsIn(oprStack,units[count],[](unit _unit, std::stack<unit> & oprStack){
                return oprStack.top().type != _type::_openBrt;
            });
            break;
        case _type::_openBrt:  
            checkCloseBrt(units,count);
        case _type::_coreFunc:
        case _type::_func:
            oprStack.push(units[count]);
            break;
        case _type::_closeBrt:
            parceCloseBrt(oprStack,units[count],units[count].name[0]);
            break; 
        case _type::_opr:
            getUnitsIn(oprStack,units[count],[](unit _unit, std::stack<unit> & oprStack){
                return  ((oprStack.top().type == _type::_opr && _unit.prior <= oprStack.top().prior) 
                    || oprStack.top().type == _type::_func 
                    || oprStack.top().type == _type::_coreFunc 
                );
            });
            oprStack.push(units[count]);
            break;
        case _type::_semicolon:
            getUnitsIn(oprStack,units.back(),[](unit _unit, std::stack<unit> & oprStack){ 
                return oprStack.size() != 0; 
            });
            break;
        }
    }
    getUnitsIn(oprStack,units.back(),[](unit _unit, std::stack<unit> & oprStack){ 
        return oprStack.size() != 0; 
    });
}

_units Parcer::getTokens() { 
    return _tokens; 
}

void Parcer::parceVarInit(_units & units,environment & env, int & count){
    if(env.have(units[count+1].name)){
        throw "var \"" + units[count+1].name + "\" already defined";
    }    
    unit newVar(_type::_var,units[count+1].name);
    if(units[count+2].name == "="){
        if(units[count+3].name == "{"){
            newVar.type = _type::_list;
          //  count++;
        }
        for(int i = count; i < units.size(); i++){
            if(units[i].name == newVar.name){
                units[i].type = newVar.type;
            }
        }
        if(newVar.type == _type::_list){
            parceListInit(newVar,units,env,count);
        }
        else{
            count+=3;
            while(units[count].type != _type::_semicolon){
                newVar._childs.push_back(units[count]);
                units.erase(units.begin()+count);
            }
        }
    }
    env.add(newVar);
}

void Parcer::parceListInit(unit & newUnit, _units & units,environment & env, int & count){
    int stopBrt = checkCloseBrt(units,count + 3);
    count+=4;
    while(count != stopBrt + 1){
        newUnit._childs.push_back(unit());
        //if(units[count].name == "{"){
        //    newUnit._childs.back().type = _type::_list;
        //    parceListInit(newUnit._childs[newUnit._childs.size() - 1 ],units,env,count);
        //}
        while(units[count].type != _type::_special){
            if(units[count].type == units[stopBrt].type){
                break;
            }
            newUnit._childs[newUnit._childs.size() - 1]._childs.push_back(units[count]);
            units.erase(units.begin()+count);
            stopBrt--;
           // count++;
        }
        count++;
    }
}

void Parcer::getUnitsIn(std::stack<unit> & oprStack, unit curUnit, condFunc func){
    while(oprStack.size() && func(curUnit, oprStack)){
        _tokens.push_back(oprStack.top());
        oprStack.pop();
    }
}

int Parcer::checkCloseBrt(_units & units, int position){
    int result;
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
            result = count;
        }
    }
    if(openChars != 0){
        throw std::string("not found closing character \"" + brtType + "\"");
    }
    return result;
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