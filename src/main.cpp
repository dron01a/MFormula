#include "lexer.h"
#include "parcer.h"

int main(){
    environment env;
    Lexer lex("1 + log(30+3,3-1) + sin(30*pi)", env);
    _units u = lex.getUnits();
    Parcer par(u,env);
    u = par.getTokens();
    return 0; 
}
