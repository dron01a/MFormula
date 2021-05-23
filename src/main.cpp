#include <iostream>

#include "parce.h"
#include "calc.h"

std::string example = "16^(1/2)";//"sin(30.0*(3.14159265/180))";///"sin((15+(30/2))*pi/180)"; // for example 

double eval(exmpUnits units);

int main(){
    exmpChars test = parceExmpl(example);
    exmpUnits t = parceChars(test);
    exmpUnits result;
    creatrePostfix(t,result);
    std::cout << eval(result)<< std::endl;
    return 0;
}

double eval(exmpUnits units){
    std::stack<double> nums;
    for(int count = 0; count < units.size(); count++){
        switch (units[count].type){
        case _num:
            nums.push(std::strtod(units[count].exp.c_str(),nullptr));
            break;
        case _func:
        case _opr:
            nums.push(calcUnits(nums,units[count].exp));
        break;
        }
    }
    return nums.top();
}

