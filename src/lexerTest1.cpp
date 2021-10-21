#include "lexer.h"
#include "parcer.h"

int main(){
    environment env;
    Lexer lex("print(25-3+4);", env);
    _units u = lex.getUnits();
    Parcer par(u,env);
    u = par.getTokens();
    return 0; 
}