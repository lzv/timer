#ifndef _EXCEPTIONS_
#define _EXCEPTIONS_

#include <iostream>

using std::cout;
using std::wcout;
using std::endl;
using std::flush;
using std::wstring;

class print_message_exception {

	public:
		const char * system_message;
		const wchar_t * my_message;

		print_message_exception (const char * msg = 0, const wchar_t * mmsg = 0) : system_message(msg), my_message(mmsg) {};
		virtual void print_message () const {
			wcout << flush;
			cout << flush;
			if (my_message) wcout << my_message << (system_message ? L": " : L".") << flush;
			if (system_message) cout << system_message << flush;
			if (my_message or system_message) cout << endl << flush;
		}
};

class storage_open_error : public print_message_exception {

	public:
		storage_open_error (const char * msg = 0, const wchar_t * rmsg = 0) : print_message_exception(msg, (rmsg ? rmsg : L"Произошла ошибка при открытии хранилища данных")) {};
};

class sqlite_open_error : public storage_open_error {

	public:
		const char * db_file_name;

		sqlite_open_error (const char * msg = 0, const char * db_name = 0, const wchar_t * rmsg = 0) : storage_open_error(msg, (rmsg ? rmsg : L"Произошла ошибка при открытии файла БД")), db_file_name(db_name) {};
		void print_message () const {
			storage_open_error::print_message();
			if (db_file_name) cout << "Имя файла: " << db_file_name << endl << flush;
		};
};

class storage_query_error : public print_message_exception {

	public:
		storage_query_error (const char * msg = 0, const wchar_t * rmsg = 0) : print_message_exception(msg, (rmsg ? rmsg : L"Произошла ошибка при запросе к хранилищу данных")) {};
};

class sqlite_query_error : public storage_query_error {

	public:
		const char * query_text;

		sqlite_query_error (const char * msg = 0, const wchar_t * rmsg = 0, const char * qt = 0) : storage_query_error(msg, (rmsg ? rmsg : L"Произошла ошибка при запросе к файлу БД")), query_text(qt) {};
		void print_message () const {
			storage_query_error::print_message();
			if (query_text) cout << "SQL-запрос: " << query_text << endl << flush;
		};
};

// Следующие классы исключений для ошибок, при которых не нужно завершать программу. 
class storage_operations {

	public:
		wstring message;

		storage_operations (wstring var_message = L"") : message(var_message) {};
		void print_message () {wcout << message << endl << flush;};
	
};

class update_storage_error : public storage_operations {

	public:
		update_storage_error (wstring var_message = L"Ошибка обновления данных.") : storage_operations(var_message) {};
};

class add_to_storage_error : public storage_operations {

	public:
		add_to_storage_error (wstring var_message = L"Ошибка добавления данных.") : storage_operations(var_message) {};
};

#endif 
