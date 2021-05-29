#include <iostream>

#include "parce.h"
#include "calc.h"

double eval(exmpUnits units);

int main(int argc, char **argv){
    std::string example("sin(30)--+sin(30)+1");
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
            nums.push(calcUnits(nums,units[count].exp, units[count].prior));
        break;
        }
    }
    return nums.top();
}

