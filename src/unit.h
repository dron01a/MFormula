#ifndef ____UNIT__H____
#define ____UNIT__H____

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <stack>
#include <cctype>
#include <map>
#include <algorithm>
#include <utility>
#include "math.h"

#define NPOS std::string::npos

namespace mf {

	class lexer;
	struct unit;

	typedef std::vector<unit> unit_vector;
	typedef std::stack<unit> unit_stack;

	enum class _type : uint8_t {
		_opr = 0,
		_func,
		_core_func,
	
		_list,
		_string,
		_num,
		_bool,
		_char,
		_var,
		_self,
		_const,

		_open_c_brt, // {
		_open_r_brt, // (
		_open_s_brt, // [
	
		_close_c_brt, // }
		_close_r_brt, // )
		_close_s_brt, // ]

		_special,
		_semicolon,
		_colon,  // :
	
		_var_init,
		_self_init,
		_const_init,
		_function_init,

		_if,
		_else,
		_while,
		_for,
		_switch,
		_case,
		_default,
		_return,
		_continue,
		_break,

		_indentf,
	
		_include,
	
		_pointer,
	
		_type,
		_type_init,
		_type_var,
		_type_constr,
		_type_field_call_opr,
	
		_this,
		_point_opr,
	
		_end_of_code
	};

	enum class obj_env_type {
	    _obj = 0,
	    _type
	};

	struct unit {
		unit(unit&& moved) = default;
		unit(const unit & copy) = default;
	    unit(_type t, std::string s, int pos = 0);
		unit(_type t);
	    unit(long double _num);
	    unit(bool _val);
	    unit(){};
		~unit();
		
		unit& operator=(const unit & copy) = default;
		unit& operator=(unit&& moved) = default;

	    unit & operator[](int position);
	    
	    unit operator+(unit & _unit) const;
	    unit operator-(unit & _unit) const;
	    unit operator*(unit & _unit) const;
	    unit operator/(unit & _unit) const;
	    unit operator%(unit & _unit) const;
	    unit operator^(unit & _unit) const;
	
	    unit increment();
	    unit decrement();
	    
	    void operator+=(unit & _unit);
	    void operator-=(unit & _unit);
	    void operator*=(unit & _unit);
	    void operator/=(unit & _unit);
	    void operator%=(unit & _unit);
	    void operator^=(unit & _unit);
	
	    // операции сравнения 
	    bool operator==(const unit & _unit) const;
	    bool operator!=(const unit & _unit) const;
	    bool operator>(unit & _unit) const;
	    bool operator<(unit & _unit) const;
	    bool operator>=(unit & _unit) const;
	    bool operator<=(unit & _unit) const;
	    bool operator&&(unit & _unit) const;
	    bool operator||(unit & _unit) const;
	
	    bool to_bool() const;
	    long double to_double() const;
	    int to_int() const;
	    std::string to_string() const;
	
	    void assign(unit & unit);
	    int size();
	    void resize(int _newsize);
	
	    unit_vector _childs;
	    std::string name;
	    
	    long double double_val = 0; 
	    bool bool_val; 
	
	    unit * __mem = nullptr;
	
	    _type type = _type::_indentf;
	    size_t prior = 0;
	    size_t _str = 0;
	    size_t _col = 0;
		size_t stream_pos = 0;

		lexer * src = nullptr;
	};

	struct error{
	    //конструктор класса
	    error(unit _token, std::string _message) : _unit(_token), message(_message){};
	    unit _unit; 
	    std::string message;
	};
	
	// для чтения структуры unit из потока
	std::istream& operator>> (std::istream& _stream, unit & unit);
	
	// для вывода структуры unit в поток
	std::ostream& operator<< (std::ostream& _stream, unit & unit);
	
	// для вывода структуры error в поток
	std::ostream& operator<< (std::ostream& _stream, error & _error);
	
	// дает ссылку на оригинальную частицу кода 
	unit & original(unit & node);
	
	// возвращает частицу кода со значением родительского узла 
	unit value(unit & node);
	
	unit value(unit * node);

}

#endif

//                  __________        __________
//                  \--------/ <\  /> \--------/
//                   \-------\__/  \__/-------/
//                     \------\      /------/
//                             /\  /\
//							  "' \/ '"
//                      |ИМПЕРАТОР ЗАЩИТИТ|  
//                      |СЛАВА БОГУ МАШИНЕ|


//class code_unit {
	//public:

	//	// методы для установки и получения значений 

	//	void str(std::string & val);
	//	std::string str();
	//	void str(std::string & val);
	//	std::string str();
	//	void str(std::string & val);
	//	std::string str();


	//private:
	//	// параметры характеризующее значение вложенное в частицу кода
	//	std::string _string_val; // строковое значение
	//	long double _double_val = 0; // числововое значение 
	//	bool _bool_val = false; // логическое значение 
	//	code_unit * _ptr_val = nullptr; // область памяти на которую сссылается частица кода
	//	unit_vector _list_data; // внутренние члены частицы кода

	//	// параметры характерезующие положение в коде
	//	size_t prior = 0; // приоретет
	//	size_t _str = 0; // линия 
	//	size_t _col = 0; // столбец
	//	_type type = _type::_indentf; // тип 
	//};