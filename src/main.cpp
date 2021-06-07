#include <iostream>

#include "parce.h"
#include "calc.h"

double eval(std::string example);

int main(int argc, char **argv){
    std::string example(argv[1]);
    try{
        std::cout << eval(example) << std::endl;
    }
    catch(std::string _errMes){
        std::cout << _errMes <<std::endl;
    }
    return 0;
}

double eval(std::string example){
    exmpChars _chars = parceExmpl(example);
    exmpUnits _units = parceChars(_chars);
    _units = creatrePostfix(_units);
    std::stack<double> nums;
    for(int count = 0; count < _units.size(); count++){
        switch (_units[count].type){
        case _special:
            if(_units[count].exp == "pi"){
                nums.push(3.1415926535);
            }
            if(_units[count].exp == "e"){
                nums.push(2.7182818284);
            }
            break;
        case _num:
            nums.push(std::strtod(_units[count].exp.c_str(),nullptr));
            break;
        case _func:
        case _opr:
            nums.push(calcUnits(nums, _units[count].exp, _units[count].prior));
        break;
        }
    }
    return nums.top();
}

