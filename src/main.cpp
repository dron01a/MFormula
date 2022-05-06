#include <iostream>
#include <iomanip>
#include <fstream>

//#include "interpretation.h"
//
//int main(int argc, char *argv[]){
//    if(argc <= 1 ){
//        std::cout << "not valid args";
//    }
//    if(strcmp(argv[1],"-s") == 0){
//        run("print(" + std::string(argv[2]) + ")");
//    }
//    else if(strcmp(argv[1],"-f") == 0){
//        std::ifstream in( argv[2],std::ios::binary);
//        if(!in.good()){
//            std::cout << "not found file " << argv[2] << std::endl;
//        }
//        int size = in.seekg(0,std::ios::end).tellg();
//        in.seekg(0);
//        char * buf = new char[size+1];
//        in.read(buf,size);
//        buf[size] = 0;
//        run(std::string(buf));
//    }
//    return 0; 
//}

#include "lexalz.h"
#include "parse.h"

int main(){
    environment env;
    std::string _str = "for(1 = 0){a = v}";
    _units _res;
    try{
        _res = lex(_str);
        _res = parse(_res, env);   
    }
    catch(error & _error){
        printf("in line:%zu col:%zu\n error:%s", _error._unit._str,_error._unit._col, _error.message.c_str());
    }
     
    return 0;
}