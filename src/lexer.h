#ifndef __LEXALZ__
#define __LEXALZ__

#include <fstream>

#include "unit.h"
#include "stack.h"

#define NPOS std::string::npos

#define end_of_line '\n' 
#define tab '\t'
#define space ' '
#define end_of_code '\0'

namespace mf{

	// класс лексера
	class lexer  {
	public:
		lexer(std::string data) {}; // конструктор класса
		
		// Виртуальные функции

		// возвращает следующюю частицу кода
		virtual mf::unit next_lex() = 0;

		// возвращает на одну позицию назад
		virtual void step_back() = 0;

		// устанавливает позицию в потоке
		virtual void set_pos(size_t pos) = 0;

		//  Общие функции

		// возвращает предыдущюю частицу кода
		mf::unit & last_lex() { return last_unit; };

		// позиция последняя полученная частица кода
		size_t & last_pos() { return _last_pos; };

		// пропускает элементы пока не будет встречен элемент указанного типа
		void skip_to(_type target);

		// пропускает элементы пока не будет удовлетворено условие
		void skip_to(bool (*stop_condition)(_type));

		// пропускает скобки 
		void skip_brt(_type open_brt, _type close_brt);

		mf::unit last_unit; // последняя полученная частица кода

		mf::stack_frame * src_frame = nullptr; // указатель на область видимости

	private:

		size_t _last_pos = 0; // передыдущая позиция
	};

	// класс прототип содержащий функции для работы с текстом
	class text_lexer : public lexer {
	public:
		text_lexer(std::string str) : lexer(str) {};

		// возвращает следующюю частицу кода
		mf::unit next_lex();

		// возвращает следующий символ
		virtual char get_next_char() = 0;

		// возвращает текущюю позицию назад на 1 единицу
		virtual void decrem_pos() = 0;

		// сохраняет позицию
		virtual void save_pos() = 0;

	protected:

		// проверяет является ли символ разделителем
		bool is_delim(char sim);

		// возвращает тип символа 
		mf::_type get_char_type(std::string & str);

		mf::_type get_char_type(char & ch);

		// возвращает тип строки
		mf::_type get_string_type(std::string & str, stack_frame * frame);

		// обработка идентификаторов
		void read_identifier(std::string & result);

		// обработка чисел 
		void read_digit(std::string & result);

		// возвращает приотретет токена
		size_t get_priority(std::string & _token);

	private:
		char cur_char = 0; // текущий символ
		char last_char = 0; // предыдущий символ
	};

	// лексический анализатор для получения частиц кода из файла 
	class file_lexer final : public text_lexer {
	public:
		file_lexer(std::string file_name);

		// возвращает на одну позицию назад
		void step_back();

		// установка необходимой позиции
		void set_pos(size_t pos);

		// возвращает следующий символ
		char get_next_char();

		// возвращает текущюю позицию назад на 1 единицу
		void decrem_pos();

		// сохраняет позицию
		void save_pos();

	private:
		std::ifstream file; // файловый поток 
		std::string file_name; // имя файла
		size_t line; // линия
	};

	// лексический анализатор получающий чатицы кода из файла
	class string_lexer final : public text_lexer {
	public:
		// конструктор класса
		string_lexer(std::string data);

		// возвращает на одну позицию назад
		void step_back();

		// установка необходимой позиции
		void set_pos(size_t pos);

		// возвращает следующий символ
		char get_next_char();

		// возвращает текущюю позицию назад на 1 единицу
		void decrem_pos();

		// сохраняет позицию
		void save_pos();
	private:
		std::string src; // строка с исходным кодом
		size_t cur_pos; // текущая позиция в строке
	};

	// лексический анализатор для получения частиц кода из вектора
	class cached_lexer final : public lexer {
	public:
		// конструктор класса
		cached_lexer(std::string data);

		// возвращает следующюю частицу кода
		mf::unit next_lex();

		// возвращает на одну позицию назад
		void step_back();

		// установка позиции
		void set_pos(size_t pos);

		// добавляет новый элемент в вектор
		void add(mf::unit & u);

		// добавляет вектор целиком
		void add(mf::unit_vector & vect);

		// возвращает ссылку на частицы кода
		unit_vector & get_code();
	private:
		unit_vector code; // хранилище кода
	};

	// таблица ключевых слов и встроенных функций
	static std::map<std::string, _type> key_word_table = { 
		{ "if" , _type::_if },
		{ "else" , _type::_else },
		{ "for" ,  _type::_for },
		{ "while" ,  _type::_while },
		{ "switch" , _type::_switch},
		{ "case" , _type::_case },
		{ "default" , _type::_default },
		{ "break" ,  _type::_break },
		{ "return" ,  _type::_return },
		{ "continue" ,  _type::_continue },
		{ "include" ,  _type::_include },
		{ "type" ,  _type::_type_init },
		{ "var" ,  _type::_var_init },
		{ "const" ,  _type::_const_init },
		{ "self" ,  _type::_self_init },
		{ "func", _type::_function_init},
		{ "abs" ,  _type::_core_func },
		{ "arc" ,  _type::_core_func },
		{ "cos" ,  _type::_core_func },
		{ "sin" ,  _type::_core_func },
		{ "tg" ,  _type::_core_func },
		{ "ln" ,  _type::_core_func },
		{ "ctg" ,  _type::_core_func },
		{ "sqrt" ,  _type::_core_func },
		{ "arcsin" ,  _type::_core_func },
		{ "arccos" ,  _type::_core_func },
		{ "arctg" ,  _type::_core_func },
		{ "arcctg" ,  _type::_core_func },
		{ "!" ,  _type::_core_func },
		{ "log" ,  _type::_core_func },
		{ "deg" ,  _type::_core_func },
		{ "print" ,  _type::_core_func },
		{ "resize" ,  _type::_core_func },
		{ "size" ,  _type::_core_func },
		{ "push_back" ,  _type::_core_func },
		{ "system" ,  _type::_core_func },
		{ "run" ,  _type::_core_func },
		{ "exit" ,  _type::_core_func },
		{ "read" ,  _type::_core_func },
		{ "clear" ,  _type::_core_func },
		{ "env" ,  _type::_core_func },
		{ "round" ,  _type::_core_func },
		{ "defined" ,  _type::_core_func },
		{ "delete" ,  _type::_core_func },
		{ "get_type" ,  _type::_core_func },
		{ "error" ,  _type::_core_func },
		{ "true" ,  _type::_bool },
		{ "false" ,  _type::_bool },
	};

	// таблца с операциями и операторами
	static std::map<std::string, _type> char_table = {
		{ "{", _type::_open_c_brt },
		{ "(", _type::_open_r_brt },
		{ "[", _type::_open_s_brt },
		{ "}", _type::_close_c_brt },
		{ ")", _type::_close_r_brt },
		{ "]", _type::_close_s_brt },
		{ ".", _type::_type_field_call_opr },
		{ "+", _type::_opr },
		{ "-", _type::_opr },
		{ "*", _type::_opr },
		{ "/", _type::_opr },
		{ "%", _type::_opr },
		{ "=", _type::_opr },
		{ "^", _type::_opr },
		{ "~", _type::_opr },
		{ "<", _type::_opr },
		{ ">", _type::_opr },
		{ "==", _type::_opr },
		{ "!=", _type::_opr },
		{ ">=", _type::_opr },
		{ "<=", _type::_opr },
		{ "&&", _type::_opr },
		{ "||", _type::_opr },
		{ "++", _type::_opr },
		{ "--", _type::_opr },
		{ "+=", _type::_opr },
		{ "-=", _type::_opr },
		{ "*=", _type::_opr },
		{ "/=", _type::_opr },
		{ "^=", _type::_opr },
		{ "%=", _type::_opr },
		{ ",", _type::_special },
		{ ";", _type::_semicolon },
		{ ":", _type::_colon },
		{ std::string(1, end_of_code), _type::_end_of_code }
	};

};

#endif

//                  __________        __________
//                  \--------/ <\  /> \--------/
//                   \-------\__/  \__/-------/
//                     \------\      /------/
//                             /\  /\
//							  "' \/ '"
//                      |ИМПЕРАТОР ЗАЩИТИТ|  
//                      |СЛАВА БОГУ МАШИНЕ|
