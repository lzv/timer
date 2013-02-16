#ifndef _DATA_PROVIDER_
#define _DATA_PROVIDER_

#include <vector>
#include <map>
#include <string>
#include "exceptions.h"
#include <stdexcept>
#include "sqlite3.h"
#include "content_types.h"

using std::out_of_range;
using std::vector;
using std::string;
using std::wstring;
using std::map;

typedef vector <map <string, string> > Data_Rows;
typedef vector <map <string, string> > :: size_type Data_Rows_Size;

// Класс для взаимодействия между самим хранилищем данных и программой. Хотелось бы кое-что сделать на шаблонах, но возникает проблема, что шаблонный метод не может быть виртуальным. Приходится прописывать все для каждого типа данных. 
class data_provider {

	protected:
		virtual ~data_provider () {};
		template <class workC> void check_allow_add_work (workC &) throw (add_to_storage_error);
		void check_in_day_and_last_period_closed (const wstring, const datetime &) throw (add_to_storage_error);
		void check_allow_add_one_work (one_work &) throw (add_to_storage_error); 		// Установка данных по умолчанию (например, дата-время добавления). Проверка корректности данных и допустимости добавления по логике программы. Установка id с учетом уже имеющихся элементов. 
		void check_allow_add_long_work (long_work &) throw (add_to_storage_error); 			// То же.
		void check_allow_add_day (day &) throw (add_to_storage_error); 						// То же.
		void check_allow_add_time_period (time_period &) throw (add_to_storage_error); 		// То же.
		void check_allow_add_work_checked (work_checked &) throw (add_to_storage_error); 	// То же.
		template <class workC> void check_allow_update_work (workC &) throw (update_storage_error);
		void check_allow_update_one_work (one_work &) throw (update_storage_error);			// Проверка существования элемента с указанным id. Перезапись данных, которые нельзя менять, оригинальными данными. Проверка обновления по логике программы. 
		void check_allow_update_long_work (long_work &) throw (update_storage_error);		// То же.
		void check_allow_update_day (day &) throw (update_storage_error);					// То же.
		void check_allow_update_time_period (time_period &) throw (update_storage_error);	// То же.
		void check_allow_update_work_checked (work_checked &) throw (update_storage_error);	// То же.

	public:
		virtual void free () = 0; // Метод вызывается отдельно, для закрытия соединения. Актуально для тех хранилищ, соединение с которыми не нужно поддерживать все время работы программы. 
		virtual int add_one_work (one_work) = 0; 			// id не учитывается, перед добавлением необходимо вызывать метод check_allow_add_one_work. При ошибке вызывается исключение add_to_storage_error. Возвращается id нового элемента. 
		virtual int add_long_work (long_work) = 0; 		// id не учитывается, перед добавлением необходимо вызывать метод check_allow_add_long_work. При ошибке вызывается исключение add_to_storage_error. Возвращается id нового элемента. 
		virtual int add_day (day) = 0; 					// id не учитывается, перед добавлением необходимо вызывать метод check_allow_add_day. При ошибке вызывается исключение add_to_storage_error. Возвращается id нового элемента. 
		virtual int add_time_period (time_period) = 0; 	// id не учитывается, перед добавлением необходимо вызывать метод check_allow_add_time_period. При ошибке вызывается исключение add_to_storage_error. Возвращается id нового элемента. 
		virtual int add_work_checked (work_checked) = 0;	// id не учитывается, перед добавлением необходимо вызывать метод check_allow_add_work_checked. При ошибке вызывается исключение add_to_storage_error. Возвращается id нового элемента. 
		virtual void update_one_work (one_work) = 0;		// Перед обновлением необходимо вызвать check_allow_update_one_work. При ошибке вызывается исключение update_storage_error. 
		virtual void update_long_work (long_work) = 0;		// Перед обновлением необходимо вызвать check_allow_update_long_work. При ошибке вызывается исключение update_storage_error.
		virtual void update_day (day) = 0;					// Перед обновлением необходимо вызвать check_allow_update_day. При ошибке вызывается исключение update_storage_error.
		virtual void update_time_period (time_period) = 0;	// Перед обновлением необходимо вызвать check_allow_update_time_period. При ошибке вызывается исключение update_storage_error.
		virtual void update_work_checked (work_checked) = 0;// Перед обновлением необходимо вызвать check_allow_update_work_checked. При ошибке вызывается исключение update_storage_error.
		virtual void delete_one_work (int) = 0;		// Полное удаление однократного дела по id из БД, вместе с его отметками о выполнении. 
		virtual void delete_long_work (int) = 0;	// Полное удаление продолжительного дела по id из БД, вместе с его временными периодами. 
};

// Взаимодействие с файлом БД SQLite. 
class dp_sqlite : public data_provider {

	friend data_provider * dp (char);

	private:
		static dp_sqlite * obj; // Объект "одиночка". 

		// Поддержка шаблона "одиночка". 
		static dp_sqlite * get_obj ();
		static void free_obj ();

		static int callback (void *, int, char **, char **); // Функция обратного вызова для SQLite.
		static string get_ids_string_for_sql (const Data_Rows &); // Вытаскивает значения id строк и формирует из них часть SQL-запроса после WHERE.

		static const char * filename; // Имя файла с БД. В дальнейшем можно будет читать его из настроек. 
		static bool db_was_checked; // Проверяем БД один раз за время работы программы, поэтому static.

		sqlite3 * db; // Указатель на БД SQLite. 
		char * ErrMsg; // Сообщение об ошибке от библиотеки SQLite. 

		dp_sqlite (); // Открываем БД и инициализируем ее. 
		~dp_sqlite (); // Закрываем БД. 
		dp_sqlite (const dp_sqlite & obj) {}; // Запрет копирования. 
		dp_sqlite & operator= (dp_sqlite & obj) {return obj;}; // Запрет присваивания.

		void sql_query_exec (const char *, Data_Rows * const = 0) throw (sqlite_query_error); // Выполняем SQL-запрос получения данных, и если ошибка, создаем исключение. Если второй параметр 0, то выполняем запрос без получения данных. 
		void load_works (); // Загружает данные о делах из БД в кэш, если они не были загружены ранее.
		void load_days (); // Загружает данные о учитываемых днях из БД в кэш, если они не были загружены ранее.
		void load_time_periods (); // Загружает данные о временных периодах из БД в кэш, если они не были загружены ранее.
		void load_work_checkeds (); // Загружает данные о учете однократных дел из БД в кэш, если они не были загружены ранее.

	public:
		void free ();
		int add_one_work (one_work);
		int add_long_work (long_work);
		int add_day (day);
		int add_time_period (time_period);
		int add_work_checked (work_checked);
		void update_one_work (one_work);
		void update_long_work (long_work);
		void update_day (day);
		void update_time_period (time_period);
		void update_work_checked (work_checked);
		void delete_one_work (int);
		void delete_long_work (int);
};

// Функция возвращает объект, через который производятся запросы к хранилищу данных. Можно реализовать несколько хранилищ и указывать нужное через параметр res. Так же можно изменить хранилище по умолчанию сразу для всех программы.
// Для этого необходимо, что бы эта функция была единственной возможностью получить доступ к хранилищу. 
data_provider * dp (char res = 0);

#endif 
