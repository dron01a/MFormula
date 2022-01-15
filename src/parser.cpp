#include "parser.h"

Parser::Parser(_units & units, environment & env){
    std::stack<unit> oprStack;
    for(int count = 0; count < units.size(); count++){
        switch (units[count].type){
        case _type::_varInit:
            units[count]._childs.push_back(units[count+1]);
            _tokens.push_back(units[count]);
            parseVarInit(units,env,count);
            break;
        case _type::_functionInit:
            _tokens.push_back(parseFuncInit(units,env,count));
            break;
        case _type::_if:
            _tokens.push_back(parseIF(units,env,count));
            break;
        case _type::_while:
            _tokens.push_back(parseWhile(units,env,count));
            break;
        case _type::_for:
            _tokens.push_back(parseFor(units,env,count));
            break;
        case _type::_break:
        case _type::_continue:
        case _type::_bool:
        case _type::_string:
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
        case _type::_list:
            oprStack.push(units[count]);
            break;
        case _type::_closeBrt:
            parseCloseBrt(oprStack,units[count],units[count].name[0]);
            break; 
        case _type::_return:
        case _type::_opr:
            getUnitsIn(oprStack,units[count],[](unit _unit, std::stack<unit> & oprStack){
                return  ((oprStack.top().type == _type::_opr && _unit.prior <= oprStack.top().prior) 
                    || oprStack.top().type == _type::_func 
                    || oprStack.top().type == _type::_coreFunc 
                    || oprStack.top().type == _type::_list
                );
            });
            oprStack.push(units[count]);
            break;
        case _type::_semicolon:
            getUnitsIn(oprStack,units.back(),[](unit _unit, std::stack<unit> & oprStack){ 
                return oprStack.size() != 0; 
            });
            break;
        case _type::_indentf:
            if(env.have(units[count].name)){
                units[count].type = env.get(units[count].name).type;
                count--;
            }
            break;
        }
    }
    getUnitsIn(oprStack,units.back(),[](unit _unit, std::stack<unit> & oprStack){ 
        return oprStack.size() != 0; 
    });
}

_units Parser::getTokens() { 
    return _tokens; 
}

void Parser::parseVarInit(_units & units,environment & env, int & count){
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
            parseListInit(newVar,units,env,count);
        }
        else{
            count+=3;
            while(units[count].type != _type::_semicolon){
                //if(units[count].type == _type::_special && units[count+1].type != _type::_varInit){
                //    break;
                //}
                newVar._childs.push_back(units[count]);
                units.erase(units.begin()+count);
            }
        }
    }
    else{
        if(units[count + 2].type == _type::_semicolon ){
            count+=2;
        }
        if(units[count + 2].type == _type::_special ){
            count+=3;
        }
    }
    env.add(newVar);
}

void Parser::parseListInit(unit & newUnit, _units & units,environment & env, int & count){
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

void Parser::getUnitsIn(std::stack<unit> & oprStack, unit curUnit, condFunc func){
    while(oprStack.size() && func(curUnit, oprStack)){
        _tokens.push_back(oprStack.top());
        oprStack.pop();
    }
}

void Parser::parseCondition(_units & units,environment & env, int & count){
    int curPos = count;
    int stopBrt = checkCloseBrt(units,count + 1);
    units[curPos]._childs.push_back(unit());
    units[curPos]._childs.push_back(unit());
    count++;
    units[curPos][0]._childs = {units.begin() + count + 1, units.end() - (units.size() - stopBrt)};
    count = stopBrt+1;
    stopBrt = checkCloseBrt(units,count);
    units[curPos][1]._childs = {units.begin() + count + 1, units.end() - (units.size() - stopBrt)};
    count = stopBrt;
}


unit Parser::parseIF(_units & units,environment & env, int & count){
    int curPos = count;
    parseCondition(units,env,count);
    count++;
    if(units[count].type == _type::_else){
        int stopBrt = checkCloseBrt(units,count + 1);
        units[curPos]._childs.push_back(unit());
        units[curPos][2]._childs = {units.begin() + count + 2, units.end() - (units.size() - stopBrt)};
        count = stopBrt+1;
    }
    count--;
    return units[curPos];
}

unit Parser::parseWhile(_units & units,environment & env, int & count){
    int curPos = count;
    parseCondition(units,env,count);
    return units[curPos];
}

unit Parser::parseFor(_units & units,environment & env, int & count){
    int curPos = count;
    int stopBrt = checkCloseBrt(units,count + 1);
    _units condition = {units.begin() + count + 1, units.end() - (units.size() - stopBrt)};
    auto _del1 = std::find_if(condition.begin(), condition.end(), [](unit _a){ return _a.name == ";"; });
    auto _del2 = std::find_if(_del1+1, condition.end(), [](unit _a){ return _a.name == ";"; });
    units[curPos]._childs.push_back(unit());
    units[curPos]._childs.push_back(unit());
    units[curPos]._childs.push_back(unit());
    units[curPos]._childs.push_back(unit());
    units[curPos][0]._childs = {condition.begin() + 1, condition.end() - (condition.size() - std::distance(condition.begin(),_del1)) + 1};
    units[curPos][1]._childs = {_del1 + 1, condition.end() - (condition.size() - std::distance(condition.begin(),_del2)) + 1};
    units[curPos][2]._childs = {_del2 + 1, condition.end()};
    count = stopBrt+1;
    stopBrt = checkCloseBrt(units,count);
    units[curPos][3]._childs = {units.begin() + count + 1, units.end() - (units.size() - stopBrt)};
    count = stopBrt;
    return units[curPos];
}

unit Parser::parseFuncInit(_units & units,environment & env, int & count){
    int curPos = count;
    count+=2;
    units[curPos + 1].type = _type::_func;
    for(int i = count; i < units.size(); i++){
        if(units[i].name == units[curPos + 1].name){
            units[i].type = units[curPos + 1].type;
        }
    }
    count--;
    parseCondition(units,env,count);
    units[curPos + 1][0]._childs.push_back(unit(_type::_special, ";")); 
    units[curPos]._childs.push_back(units[curPos + 1]);
    env.add(units[curPos + 1]);
    return units[curPos];
}

int Parser::checkCloseBrt(_units & units, int position){
    int result;
    std::string brtType;
    int openChars = 1; 
    switch(units[position].name[0]){
        case '(':brtType = ")";break;
        case '[':brtType = "]";break;
        case '{':brtType = "}";break;
    }
    for(int count = position + 1;; count++){
        if(units[count].name == units[position].name){
            openChars++;
        }
        if(units[count].name == brtType){
            openChars--;
        }
        if(openChars == 0){
            result = count;
            break;
        }
    }
    if(openChars != 0){
        throw std::string("not found closing character \"" + brtType + "\"");
    }
    return result;
}

void Parser::parseCloseBrt(std::stack<unit> & oprStack, unit curUnit, char _type){
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