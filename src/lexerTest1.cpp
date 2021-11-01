#include "lexer.h"
#include "parcer.h"

int main(){
    environment env;
    Lexer lex("log(sin(2-1),1-0)", env);
    _units u = lex.getUnits();
    Parcer par(u,env);
    u = par.getTokens();
    return 0; 
}
