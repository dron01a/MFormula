#include "lexer.h"

mf::_type mf::text_lexer::get_char_type(std::string & str) {
	std::map<std::string, _type>::iterator ch = char_table.find(str);
	if (ch != char_table.end()) {
		return ch->second;
	}
	return _type::_indentf;
}

mf::_type mf::text_lexer::get_char_type(char & ch) {
	std::map<std::string, _type>::iterator it = char_table.find(std::string(1, ch));
	if (it != char_table.end()) {
		return it->second;
	}
	return _type::_indentf;
}

mf::_type mf::text_lexer::get_string_type(std::string & str, stack_frame * frame) {
	std::map<std::string, _type>::iterator kw = key_word_table.find(str);
	if (kw != key_word_table.end()) {
		return kw->second;
	}
	if (frame != nullptr) {
		return frame->get_type(str);
	}
	return _type::_indentf;
}

void mf::text_lexer::read_identifier(std::string & result){
	while (!is_delim(cur_char)) { // получаем символы пока не встретим разделитель
		result.push_back(cur_char);
		last_char = cur_char;
		cur_char = get_next_char(); // получаем следующий символ
	}
	decrem_pos();
}

void mf::text_lexer::read_digit(std::string & result) {
	while (std::isdigit(cur_char)) {
		last_char = cur_char;
		result.push_back(std::move(cur_char));
		cur_char = get_next_char(); // получаем следующий символ
		switch (cur_char) {
		case 'e':
			last_char = cur_char;
			cur_char = get_next_char(); // получаем следующий символ
			if (cur_char != '+' && cur_char != '-') {
				throw "неверная запись мантисы!";
			}
			result.push_back(std::move(last_char)); // e 
			result.push_back(std::move(cur_char));  // + или -
			cur_char = get_next_char(); // получаем следующий символ
			break;
		case '.':
			result.push_back(std::move(cur_char));
			cur_char = get_next_char(); // получаем следующий символ
			break;
		}
	}
	last_char = cur_char;
	decrem_pos();
}

mf::unit mf::text_lexer::next_lex(){
	std::string token;
	token.reserve(128);
	cur_char = get_next_char();  // получаем следующий символ
	while (cur_char == space || cur_char == tab || cur_char == end_of_line) { // пропускаем пробелы и конец строки
		cur_char = get_next_char();  // получаем следующий символ
	}
	save_pos();
	if (std::isalpha(cur_char) || cur_char == '_') {
		read_identifier(token);
		last_unit = mf::unit(get_string_type(token, src_frame), token, last_pos());
		return last_unit;
	}
	else if (std::isdigit(cur_char)) {
		read_digit(token);
		last_unit = mf::unit(_type::_num, token, last_pos());
		return last_unit;
	}
	else if (cur_char == '\"') {
		cur_char = get_next_char(); // получаем следующий символ
		while (cur_char != '\"') {
			token.push_back(std::move(cur_char));
			cur_char = get_next_char(); // получаем следующий символ
		}
		last_char = cur_char;
		last_unit = mf::unit(_type::_string, token, last_pos());
		return last_unit;
	}
	else {
		switch (cur_char) {
		case '/':
			token += cur_char;
			cur_char = get_next_char();  // получаем следующий символ
			switch (cur_char) {
			case '=':
				token.push_back(std::move(cur_char));
				cur_char = get_next_char(); // получаем следующий символ
				break;
			case '/':
				token.clear();
				while (cur_char != end_of_line) { // движемся до конца строки
					cur_char = get_next_char();  // получаем следующий символ
					if (cur_char == end_of_code) {
						last_unit = unit(_type::_end_of_code, "");
						return last_unit;
					}
				}
				return next_lex();
			case '*':
				token.clear();
				while (cur_char != end_of_code) { // движемся до момента пока не встретим '*'
					cur_char = get_next_char();  // получаем следующий символ
					if (cur_char == '*') {
						cur_char = get_next_char();  // получаем следующий символ
						if (cur_char == '/') {
							cur_char = get_next_char(); // получаем следующий символ
							last_unit = next_lex();
							return last_unit;
						}
					}
				}
				break;
			}
			break;
		case '*':
			token.push_back(cur_char);
			cur_char = get_next_char();  // получаем следующий символ
			if (cur_char == '=') {
				token.push_back(cur_char);
				cur_char = get_next_char();  // получаем следующий символ
			}
			break;
		case '&':
		case '>':
		case '<':
		case '|':
		case '+':
			token.push_back(std::move(cur_char));
			cur_char = get_next_char(); // получаем следующий символ
			if (cur_char == '=' || cur_char == token.back()) {
				token.push_back(std::move(cur_char));
				cur_char = get_next_char(); // получаем следующий символ
			}
			break;
		case '!':
		case '^':
		case '%':
		case '=':
			token.push_back(std::move(cur_char));
			cur_char = get_next_char();  // получаем следующий символ
			if (cur_char == '=') {
				token.push_back(std::move(cur_char));
				cur_char = get_next_char();  // получаем следующий символ
			}
			break;
		case end_of_code:
			last_unit = unit(_type::_end_of_code, "");
			return last_unit;
		case '.':
		case ',':
		case ';':
		case '(':
		case ')':
		case '{':
		case '}':
		case '[':
		case ']':
		case ':':
			token.push_back(std::move(cur_char));
			cur_char = get_next_char();  // получаем следующий символ
			break;
		case '-':
			token.push_back(std::move(cur_char));
			cur_char = get_next_char(); // получаем следующий символ
			if (cur_char == '=' || cur_char == token.back()) {
				token.push_back(std::move(cur_char));
				cur_char = get_next_char(); // получаем следующий символ
			}
			else if (get_char_type(last_char) == _type::_opr
				|| get_char_type(last_char) == _type::_open_c_brt
				|| get_char_type(last_char) == _type::_open_r_brt
				|| get_char_type(last_char) == _type::_open_s_brt
				|| get_char_type(last_char) == _type::_special
				) {
				token.back() = '~';
			}
			break;
		default:
			throw "Недопустимый символ";
		}
	}
	last_unit.prior = get_priority(token);
	last_unit.stream_pos = last_pos();
	last_unit.type = get_char_type(token); // получаем тип 
	last_char = token.back();
	last_unit.name = std::move(token);
	decrem_pos();
	return last_unit;
}

bool mf::text_lexer::is_delim(char sim){
	return std::string("|&!{}[]()+~-*/^%,;:<>=\n \t.").find(sim) != NPOS;
}

size_t mf::text_lexer::get_priority(std::string & _token) {
	if (_token == "==" || _token == "!=" || _token == ">=" || _token == "<=" || _token == ">" || _token == "<") {
		return 1;
	}
	if (_token == "&&" || _token == "||") {
		return 0;
	}
	if (_token == "+" || _token == "-") {
		return 2;
	}
	if (_token == "*" || _token == "/" || _token == "%") {
		return 3;
	}
	if (_token == "^" || _token == "~" || _token == "++" || _token == "--") {
		return 4;
	}
	if (_token == "!") {
		return 5;
	}
	return 0;
}

void mf::file_lexer::step_back(){
	this->file.seekg(this->last_pos() - 1);
}

void mf::file_lexer::set_pos(size_t pos){
	file.seekg(pos, std::ios::beg);
	last_pos() = file.tellg();
}

char mf::file_lexer::get_next_char(){
	char result = file.get();
	if (file.eof()) {
		result = end_of_code;
		file.clear();
		file.seekg(1, std::ios::end);
	}
	else if (result == end_of_line) {
		line++;
	}
	else if (result == '\r') {
		result = get_next_char();
	}
	return result;
}

void mf::file_lexer::decrem_pos(){
	file.seekg(-1, std::ios::cur);
}

void mf::file_lexer::save_pos(){
	last_pos() = file.tellg();
	last_lex()._str = line;
}

mf::file_lexer::file_lexer(std::string file_name) : text_lexer(file_name) {
	file.open(file_name, std::ios::binary | std::ios::in);
	this->file_name = file_name;
	file.clear();
}

mf::string_lexer::string_lexer(std::string data) : text_lexer(data) {
	this->src = data;
	this->src.push_back(end_of_code);
}

void mf::string_lexer::step_back(){
	cur_pos = last_pos() - 1;
}

void mf::string_lexer::set_pos(size_t pos){
	cur_pos = pos;
}

char mf::string_lexer::get_next_char(){
	return src[cur_pos++];
}

void mf::string_lexer::decrem_pos(){
	cur_pos--;
}

void mf::string_lexer::save_pos(){
	last_pos() = cur_pos;
}

mf::cached_lexer::cached_lexer(std::string data) : lexer(data) {}

mf::unit mf::cached_lexer::next_lex() {
	last_unit = code[last_pos()++];
	return last_unit;
}

void mf::cached_lexer::step_back() {
	last_pos()--;
}

void mf::cached_lexer::set_pos(size_t pos) {
	this->last_pos() = pos;
}

void mf::cached_lexer::add(mf::unit & u) {
	code.push_back(u);
}

void mf::cached_lexer::add(mf::unit_vector & vect) {
	code = vect;
}

mf::unit_vector & mf::cached_lexer::get_code() {
	return code;
}

void mf::lexer::skip_to(_type target){
	while (this->last_lex().type != target) {
		this->next_lex();
	}
}

void mf::lexer::skip_to(bool(*stop_condition)(_type)){
	while (stop_condition(this->last_lex().type)){
		switch (this->last_lex().type){
		case _type::_open_c_brt:
			this->skip_brt(_type::_open_c_brt, _type::_close_c_brt);
	//		this->next_lex();
			break;
		case _type::_open_r_brt:
			this->skip_brt(_type::_open_r_brt, _type::_close_r_brt);
		//	this->next_lex();
			break;
		case _type::_open_s_brt:
			this->skip_brt(_type::_open_s_brt, _type::_close_s_brt);
	//		this->next_lex();
			break;
		default:
			this->next_lex();
			break;
		}
	}
}

void mf::lexer::skip_brt(_type open_brt, _type close_brt){
	size_t open_chars = 1;
	this->next_lex();
	while (open_chars != 0) {
		if (this->last_lex().type == open_brt) { // если встречена открывающая скобка
			open_chars++;
		}
		else if (this->last_lex().type == close_brt) { // если встречена закрывающая скобка
			open_chars--;
		}
		this->next_lex(); // получаем следующюю частицу кода
	}
}
