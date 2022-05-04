#include "parser.h"

Parser::Parser(_units & units, environment & env){
    std::stack<unit> oprStack;
    for(int count = 0; count < units.size(); count++){
        switch (units[count].type){
        case _type::_varInit:
            _tokens.push_back(parseVarInit(units,env,count));
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
        case _type::_list:
            _tokens.push_back(units[count]);
            break;
        case _type::_special: 
            getUnitsIn(oprStack,units[count],[](unit _unit, std::stack<unit> & oprStack){
                return oprStack.top().type != _type::_openBrt;
            });
            break;
        case _type::_openBrt: 
            if(units[count].name == "{"){
                count--;
                unit newList;
                newList.type = _type::_list;
                newList._childs = parseList(parseContext(units,count));
                oprStack.push(newList);
            }
            else{
                checkCloseBrt(units,count);
                oprStack.push(units[count]); 
            } 
            break;
        case _type::_coreFunc:
        case _type::_func:
            oprStack.push(units[count]);
            break;
        case _type::_closeBrt:
            parseCloseBrt(oprStack,units[count],units[count].name[0]);
            if(units[count].name == "]"){
                units[count].name = "[]";
                units[count].type = _type::_opr;
                units[count].prior = 9;
                _tokens.push_back(units[count]);

            }
            break; 
        case _type::_include:
            if(units[count + 1].type != _type::_string){
                throw "error type!!!";
            }
            includeFile(units[count+1].name,env);
            count+=2;
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

unit Parser::parseVarInit(_units & units,environment & env, int & count){
    if(env.have(units[count+1].name)){
        throw "var \"" + units[count+1].name + "\" already defined";
    }    
    int curPos = count;
    count++;
    while(units[count].type != _type::_semicolon){
        unit newVar(_type::_var,units[count].name);
        if(units[count+1].name == "="){
            if(units[count+2].name == "{"){
                newVar.type = _type::_list;
                count++;
                newVar._childs  = parseContext(units,count);
                newVar._childs = parseList(newVar._childs);
            }
            else{
                count+=2;
                while(units[count].type != _type::_semicolon){
                    if(units[count].type == _type::_func || units[count].type == _type::_coreFunc){
                        newVar._childs.push_back(units[count]);
                        newVar._childs.push_back(units[count + 1]);
                        _units args = parseContext(units,count);
                        newVar._childs.insert(newVar._childs.end(), args.begin(), args.end());
                    }
                    if(units[count].type == _type::_special && units[count+1].type == _type::_indentf){
                        count++;
                        break;
                    }
                    newVar._childs.push_back(units[count]);
                    count++;
                }
            }
        }
        env.add(newVar);
        units[curPos]._childs.push_back(newVar);
        if(count < units.size() - 1){
            switch (units[count + 1].type){
            case _type::_special:
                count+=2;
                break;
            case _type::_semicolon:
                count++;
                break;
            }
        }
        
    }
    if(units[count + 2].type == _type::_semicolon ){
        count+=2;
    }
    return units[curPos];
}

_units Parser::parseList(_units units){
    _units result;
    if(units.size() != 0){
        result.push_back(unit());
    }
    for(int _cnt = 0; _cnt < units.size(); _cnt++){
        if(units[_cnt].type == _type::_special){
            result.push_back(unit());
            continue;
        }
        if(units[_cnt].name == "{"){
            _cnt--;
            result[result.size()-1].type = _type::_list;
            result[result.size()-1]._childs = parseList(parseContext(units,_cnt));
            continue;
        }
        result[result.size()-1]._childs.push_back(units[_cnt]);
    }
    return result; 
}

void Parser::getUnitsIn(std::stack<unit> & oprStack, unit curUnit, condFunc func){
    while(oprStack.size() && func(curUnit, oprStack)){
        _tokens.push_back(oprStack.top());
        oprStack.pop();
    }
}

void Parser::parseCondition(_units & units,environment & env, int & count){
    int curPos = count;
    units[count]._childs.push_back(unit());
    units[count]._childs.push_back(unit());
    units[curPos][0]._childs = parseContext(units,count); 
    units[curPos][1]._childs = parseContext(units,count); 
}

_units Parser::parseContext(_units & units, int & count){
    count++;
    int stopBrt = checkCloseBrt(units,count);
    _units result = {units.begin() + count + 1, units.end() - (units.size() - stopBrt)};
    count = stopBrt;
    return result;
}

unit Parser::parseIF(_units & units,environment & env, int & count){
    int curPos = count;
    parseCondition(units,env,count);
    count++;
    if(units[count].type == _type::_else){
        units[curPos]._childs.push_back(unit());
        units[curPos][2]._childs = parseContext(units,count); 
        count++;
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
    count = stopBrt;
    auto _del1 = std::find_if(condition.begin(), condition.end(), [](unit _a){ return _a.name == ";"; });
    auto _del2 = std::find_if(_del1+1, condition.end(), [](unit _a){ return _a.name == ";"; });
    units[curPos]._childs.push_back(unit());
    units[curPos]._childs.push_back(unit());
    units[curPos]._childs.push_back(unit());
    units[curPos]._childs.push_back(unit());
    units[curPos][0]._childs = {condition.begin() + 1, condition.end() - (condition.size() - std::distance(condition.begin(),_del1)) + 1};
    units[curPos][1]._childs = {_del1 + 1, condition.end() - (condition.size() - std::distance(condition.begin(),_del2)) + 1};
    units[curPos][2]._childs = {_del2 + 1, condition.end()};
    units[curPos][3]._childs = parseContext(units,count); 
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
    units[curPos + 1][0]._childs.push_back(unit(_type::_semicolon, ";")); 
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
        case '[':brtType = "]";break;      //units[position].type = _type::_sqrBrtOpen;break;
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

void includeFile(std::string _file, environment & env){
    std::ifstream in(_file,std::ios::binary);
    if(!in.good()){
        throw "not found file " + _file;
    }
    int size = in.seekg(0,std::ios::end).tellg();
    in.seekg(0);
    char * buf = new char[size+1];
    in.read(buf,size);
    buf[size] = 0;
    Lexer lex(buf, env);
    _units codeTokens = lex.getUnits();
    Parser parser(codeTokens, env);
}