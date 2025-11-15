#include "main.h"
#include "string.h"

int main(int argc, char *argv[]){
	mf::stack_frame * frame = nullptr;
	mf::lexer * lex = nullptr;
	mf::unit_vector result;
	//setlocale(LC_ALL, "Ru-ru");
	if (argc == 1) {
		while (true) {
			std::string source;
			try {
				std::cout << "mf > ";
				getline(std::cin, source);
				lex = new mf::string_lexer(source += ";");
				frame = new mf::stack_frame(lex);
				result = mf::code_exec(lex, frame, [](mf::unit & u) { return u.type != mf::_type::_semicolon; });
				if(result.size() != 0 ){
				    if(result[0].type == mf::_type::_num || result[0].type == mf::_type::_string 
						|| result[0].type == mf::_type::_var || result[0].type == mf::_type::_list){
				        std::cout << result[0] << std::endl;
				    }
				}
			}
			_ERROR(source, "-s");
		}
	}
	else {
		try {
		    if(strcmp(argv[1],"-s") == 0){
				lex = new mf::string_lexer(std::string("print("+std::string(argv[2])+")"));
		    }
		    if(strcmp(argv[1],"-f") == 0){
				lex = new mf::file_lexer(argv[2]);
		    }
			frame = new mf::stack_frame(lex);
			lex->src_frame = frame;
			result = mf::code_exec(lex, frame, [](mf::unit & u) { return u.type != mf::_type::_end_of_code; });
		}
		_ERROR(argv[2], argv[1]);
	}
    return 0;
}

void error_proc(mf::error & _err, std::string source, std::string mode){
    std::string _error_point;
    //std::string _buf; // temp string 
    //if(mode == "-f"){
    //    std::ifstream in = open_file(source); // file with code
    //    std::string _error_point;
    //    size_t _str = 0;
    //    while(std::getline(in, _buf)){
    //        if(_str == _err._unit._str){
    //            break;
    //        }
    //        _str++;
    //    }
    //}
    //if(mode == "-s"){
    //    _buf = source;
    //}
    //_error_point.insert(0, _buf.size(), '-');
    //_error_point.insert(_err._unit._col, "^");
    //std::cout << _err << _buf << std::endl << _error_point << std::endl;
}

//std::ifstream open_file(std::string name) {
//	std::ifstream in(name, std::ios::binary); // file with code
//	if (in.bad()) {
//		throw "Файл не найден -> " + name;
//	}
//	return in;
//}