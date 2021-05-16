#include <iostream>

#include "parce.h"

std::string example = "sin((30/2)+15)"; // for example 

int main(){
    exmpChars test = parceExmpl(example);
    exmpUnits t = parceChars(test);
    return 0;
}
