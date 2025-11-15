#ifndef __MEMORY_MODEL__
#define	__MEMORY_MODEL__

#include "unit.h"

#define IF_TRUE_COND_FLAG (1 << 0)
#define IF_FALSE_COND_FLAG (1 << 1)

#define SWITCH_TRUE_RESULT_FLAG (1 << 2)
#define SWITCH_NOT_BREAK_FLAG (1 << 3)


namespace mf {

	class lexer; // передварительное объявление класса из lexer.h

	class stack_frame {
	public:
		// конструктор класса 
		stack_frame() {};

		stack_frame(lexer * lex);

		stack_frame(stack_frame * perv);

		stack_frame(lexer & lex, stack_frame & perv);

		// возвращает объявленные переменные и функции
		unit_vector & defined();

		// возвращает частицу по имени 
		unit & get(std::string _name, obj_env_type type);

		// возвращает функции в соответсвии с перегрузкой 
		unit * get_func_owerload(std::string name, size_t par_count);

		// проверка наличия переменной, функции или типа
		bool have(std::string _name, obj_env_type type, bool global_search = true);

		// добавить переменную или функцию, новый тип
		void add(unit & _unit, obj_env_type type);

		// возвращает тип 
		_type get_type(std::string name);

		// возвращет предыдущий кадр
		stack_frame * get_perv();

		// удаляет переменную
		void delete_var(std::string name);

		unit_vector buff; // буфер для выражений 

		// устанавливает флаг/флаги
		void set_flag(uint8_t flag);

		// сбрасывает флаг/флаги
		void reset_flag(uint8_t flag);

		// проверяет наличие флага/флагов
		bool check_flag(uint8_t flag);

	private:
		unit_vector _defined; // объявленные переменные и функции
		unit_vector types; // объявленые типы
		stack_frame * perv = nullptr; // передыдущий кадр стека
		lexer * src = nullptr; // источник
		uint8_t flags = 0; // флаги и прочие служебные биты

		// возвращает частицу по имени 
		unit & get(unit_vector & vect, std::string _name, obj_env_type type);

		// проверяет наличие
		bool have(unit_vector & vect, std::string _name, obj_env_type type, bool global_search);
	};

};

#endif // ! __MEMORY_MODEL__

//                  __________        __________
//                  \--------/ <\  /> \--------/
//                   \-------\__/  \__/-------/
//                     \------\      /------/
//                             /\  /\
//							  "' \/ '"
//                      |ИМПЕРАТОР ЗАЩИТИТ|  
//                      |СЛАВА БОГУ МАШИНЕ|