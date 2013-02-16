#ifndef _MAIN_MODULE_
#define _MAIN_MODULE_

#include <string>
#include <vector>
#include <map>
#include "content_types.h"

using std::vector;
using std::map;

class command {

	public:
		typedef wstring (*command_function) (const vector<wstring> &);

		wstring str;
		wstring help_message;
		command_function fun;

		command (const wstring & var_str = L"", const wstring & h_mess = L"", command_function var_fun = 0) : str(var_str), help_message(h_mess), fun(var_fun) {};
		bool is_valid () {return str != L"";};
};

// Это главный модуль программы, в котором будет происходить основная работа. Так же в него будут включены различные вспомогательные алгоритмы. Конечно, "суперкласс" - это не хорошо, но для данной программы не планируется неограниченного роста этого класса. Если же вдруг программа будет развиваться, можно сделать рефакторинг. 

class main_module {

	private:
		static vector<command> & commands;
		static vector<command> & commands_init ();
		
		static void print (const string &, bool = false, bool = true); // Печатаем строку с очисткой кэша по флагу. Третий параметр - перевод строки.
		static void print (const wstring &, bool = false, bool = true); // То же.
		static void print (const char *, bool = false, bool = true); // То же.
		static void print (const wchar_t *, bool = false, bool = true); // То же.

		static wstring command_exit (const vector<wstring> &);
		static wstring command_help (const vector<wstring> &);
		static wstring command_check_status (const vector<wstring> &);
		static wstring command_add (const vector<wstring> &);
		static wstring command_update (const vector<wstring> &);
		static wstring command_list (const vector<wstring> &);
		static wstring command_hide (const vector<wstring> &);
		static wstring command_start (const vector<wstring> &);
		static wstring command_stop (const vector<wstring> &);
		static wstring command_mark (const vector<wstring> &);

		template<class W> static map <int, wstring> get_names_map (const vector<W> &); // Возвращает соответствие id дела и его названия.
		template<class C> static wstring get_wstring_from_list (const wchar_t *, const vector<C> &, const wchar_t *); // Получение списка для вывода. 
		static void clear_screen (); // Очистка экрана в консоли.
		static void check_last_time_period_for_close (); // Если последний временной промежуток еще не закрыт, а учитываемый день уже закончился, закрываем промежуток на момент окончания дня.
		static void print_status (); // Печатаем текущий статус.
		static command parse_command (const wstring &); // Ищет команду в векторе commands. Если не найдено, возвращается command().
		static bool set_day_end_by_HH_MM (const wstring &, datetime &); // Определяет окончание учитываемого дня, должно быть не раньше текущего момента. Возвращает true, только если параметры корректны и значение по ссылке изменено.

	public:
		static void start (); 
};

#endif 
