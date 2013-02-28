#ifndef _CONTENT_TYPES_
#define _CONTENT_TYPES_

#include <string>
#include <vector>
#include <ctime>
#include <cctype>
#include <algorithm>

using std::string;
using std::wstring;
using std::vector;

// Количество часов, минут и секунд. 
class time_count {

	public:
		long int h;
		short int m;
		short int s;

		void init_from_seconds (long int = 0);
		bool parse_from_HH_MM_SS (const string &); // Возврат true при успешном парсинге строки HH:MM:SS, иначе false. Объект меняется только при успехе. Вместо двоеточия может быть любой символ, так как значения смотрятся по позиции в строке. 
		bool parse_from_HH_MM_SS (const wstring &); // То же.
		wstring get_wstr_for_print (char = 'n') const; // n - нормальный вид, c - компактный, w - широкий.
		long int get_seconds () const;
		time_count (long int = 0); // Создание из количества секунд. 
		time_count (long int, long int, long int);

		bool operator == (const time_count & val) const {return h == val.h and m == val.m and s == val.s;};
		bool operator != (const time_count & val) const {return h != val.h or m != val.m or s != val.s;};
		bool operator < (const time_count & val) const {return h < val.h or (h == val.h and m < val.m) or (h == val.h and m == val.m and s < val.s);};
		bool operator <= (const time_count & val) const {return h < val.h or (h == val.h and m < val.m) or (h == val.h and m == val.m and s <= val.s);};
		bool operator > (const time_count & val) const {return h > val.h or (h == val.h and m > val.m) or (h == val.h and m == val.m and s > val.s);};
		bool operator >= (const time_count & val) const {return h > val.h or (h == val.h and m > val.m) or (h == val.h and m == val.m and s >= val.s);};
		time_count operator - (const time_count & val) const;
};

// Класс для более удобной работы с датой и временем.
class datetime {

	private:
		time_t seconds;
		tm timeinfo;

	public:
		explicit datetime () : seconds(time(0)), timeinfo(* localtime(& seconds)) {}; // Текущий момент. 
		explicit datetime (time_t val) : seconds(val), timeinfo(* localtime(& val)) {};
		explicit datetime (tm val) : seconds(mktime(& val)), timeinfo(val) {};

		string get_sqlite_format () const;
		string get_print_format (bool = false) const;
		wstring w_get_print_format (bool = false) const;
		bool parse_sqlite_format (string); // Возвращается true, если успешно распарсено и значение объекта изменено, иначе false.
		void set (time_t); 
		void set (tm); 
		void test_print () const;
		bool is_zero () const {return seconds == 0;};
		time_count get_time_count () {return time_count(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);};
		time_t get_seconds () {return seconds;};
		tm get_timeinfo () {return timeinfo;};
		void change_by_seconds (long int); // Корректирует дату-время на указанное количество секунд (может быть отрицательным). 

		bool operator < (const datetime & val) const {return seconds < val.seconds;};
		bool operator > (const datetime & val) const {return seconds > val.seconds;};
		bool operator == (const datetime & val) const {return seconds == val.seconds;};
		bool operator <= (const datetime & val) const {return seconds <= val.seconds;};
		bool operator >= (const datetime & val) const {return seconds >= val.seconds;};
		bool operator != (const datetime & val) const {return seconds != val.seconds;};
		time_count operator - (const datetime & val) const {return time_count(seconds - val.seconds);};
};

// Что бы не повторять в каждом классе. 
class data_need_id {

	public:
		int id;

		explicit data_need_id (int var_id = 0) : id(var_id) {};

		bool is_valid () const {return id > 0;};
		bool operator < (const data_need_id & val) const {return id < val.id;};
		virtual bool is_correct () = 0; // Проверяется корректность только самих данных, без привязки к другим данным или логике программы. id не учитывается.
		virtual wstring get_wstring () const;
};

// Предикат для поиска по id, для использования в find_if.
class find_data_by_id {

	private:
		int need_id;

	public:
		virtual ~find_data_by_id () {};
		find_data_by_id (int val) : need_id(val) {};
		bool operator () (const data_need_id & val) {return val.id == need_id;};
};

// Здесь хранятся данные, полученные из хранилища, что бы не читать их оттуда при каждом обращении. Возможно будет занимать много ОП, но пока такое количество данных не планируется. При необходимости можно будет переделать так, что бы хранились данные только за последний учитываемый день. 
// Использовать шаблон предпологается только с типами, производными от data_need_id и не имеющими дочерних типов. На данный момент это one_work, long_work, day, time_period, work_checked.
template <class C> class data_cache {

	friend class data_provider;
	friend class dp_sqlite;

	public:
		typedef typename vector<C>::size_type size_type;

		static size_type size () {return list.size();};
		static C get_last () {return size() > 0 ? list.back() : C();};
		static C get_by_index (size_t ind) {return ind < size() ? list[ind] : C();};
		static vector<C> get_all () {return list;};

		template <class D>
		static vector<C> get_all (D C::*member, const D & value, bool need_eq = true) {
			vector<C> result;
			for (typename vector<C>::const_iterator i = begin(); i != end(); ++i)
				if ((need_eq and (*i).*member == value) or (!need_eq and (*i).*member != value)) result.push_back(*i);
			return result;
		};
		template <class D>
		static vector<C> get_all (D C::*member, const D & from, const D & to, bool need_eq = true, bool find_in = true) {
			vector<C> result;
			bool need_find = from <= to;
			for (typename vector<C>::const_iterator i = begin(); i != end(); ++i)
				if (
					need_find and
					(
						(need_eq and ((*i).*member == from or (*i).*member == to)) or
						(find_in and (*i).*member > from and (*i).*member < to) or
						(!find_in and ((*i).*member < from or (*i).*member > to))
					)
				)
					result.push_back(*i);
			return result;
		};
		static C get_by_id (int need_id) {
			typename vector<C>::const_iterator result = find_if (begin(), end(), find_data_by_id(need_id));
			return result == end() ? C() : *result;
		};
		static int get_max_id () {
			int result = 0;
			for (typename vector<C>::const_iterator i = begin(); i != end(); ++i)
				if (i->id > result) result = i->id; // Хотя сейчас сортировка по id, но потом может поменяться.
			return result;
		};

	private:
		static bool list_loaded;
		static vector<C> list;

		typedef typename vector<C>::iterator iterator;

		static iterator begin () {return list.begin();};
		static iterator end () {return list.end();};
		static void sort () {if (size() > 1) std::sort(begin(), end());};
		static iterator erase (iterator element) {return list.erase(element);};

		static iterator get_iterator_by_id (int need_id) {
			return find_if (begin(), end(), find_data_by_id(need_id));
		};
		static void add_element (C & element) {
			list.push_back(element);
			sort(); // Хотя сейчас сортировка по id, но потом может поменяться.
		};
		static bool update_element (C & element) {
			bool result = false;
			for (iterator i = begin(); i != end(); ++i)
				if (element.id == i->id) {
					*i = element;
					sort(); // Хотя сейчас сортировка по id, но потом может поменяться.
					result = true;
				}
			return result;
		};
		static void delete_element (int id) {
			iterator element = get_iterator_by_id(id);
			if (element != end()) erase(element);
		};
		template <class D>
		static void delete_by_class_member (D C::*member, const D & value) {
			size_type i, count = size();
			for (i = 0; i < count; ++i)
				if (list[i].*member == value) {
					erase(begin() + i);
					--i;
					--count;
				}
		}
};

// Жаль, что компилятор не поддерживает export template. 
template<class C> bool data_cache<C>::list_loaded = false;
template<class C> vector<C> data_cache<C>::list;

// Родительский класс для типа данных - работ. 
class work : public data_need_id {

	public:
		wstring name;
		datetime created;
		bool deleted;

		virtual ~work () {};
		explicit work (int var_id = 0, wstring var_name = L"", datetime var_created = datetime(0), bool var_deleted = false) : data_need_id(var_id), name(var_name), created(var_created), deleted(var_deleted) {};

		virtual bool is_correct () {return name != L"" and created > datetime(0);};
		virtual wstring get_wstring () const;
};

// Однократная работа. 
class one_work : public work {

	public:
		int count;

		explicit one_work (int var_id = 0, wstring var_name = L"", int var_count = 0, datetime var_created = datetime(0), bool var_deleted = false) : work(var_id, var_name, var_created, var_deleted), count(var_count) {};

		bool is_correct () {return work::is_correct() and count > 0;};
		wstring get_wstring () const;
};

// Длительная работа. 
class long_work : public work {

	public:
		int plan;

		explicit long_work (int var_id = 0, wstring var_name = L"", int var_plan = 0, datetime var_created = datetime(0), bool var_deleted = false) : work(var_id, var_name, var_created, var_deleted), plan(var_plan) {};

		bool is_correct () {return work::is_correct() and plan > 0;};
		wstring get_wstring () const;
};

// Класс для временных периодов.
class time_period : public data_need_id {

	public:
		int work_id;
		datetime start;
		datetime end;

		explicit time_period (int var_id = 0, int var_work_id = 0, datetime var_start = datetime(0), datetime var_end = datetime(0)) : data_need_id(var_id), work_id(var_work_id), start(var_start), end(var_end) {};
		
		bool is_closed () const {return !end.is_zero();};
		bool is_opened () const {return end.is_zero();};
		bool is_correct () {return work_id > 0 and start > datetime(0) and (end == datetime(0) or start < end);};
		long_work get_work () {return data_cache<long_work>::get_by_id(work_id);};
		wstring get_wstring () const;
};

// Класс для проверок однократных дел. 
class work_checked : public data_need_id {

	public:
		int work_id;
		datetime check;

		explicit work_checked (int var_id = 0, int var_work_id = 0, datetime var_check = datetime(0)) : data_need_id(var_id), work_id(var_work_id), check(var_check) {};

		bool is_correct () {return work_id > 0 and check > datetime(0);};
		one_work get_work () {return data_cache<one_work>::get_by_id(work_id);};
		wstring get_wstring () const;

		// Однократное дело можно выполнить только текущим моментом, поэтому можно сортировать по id. Если же можно будет задавать время выполнения вручную, то нужно будет переделать на сравнение по check. 
};

// Класс для учитываемого дня.
class day : public data_need_id {

	public:
		datetime start;
		datetime end;

		explicit day (int var_id = 0, datetime var_start = datetime(0), datetime var_end = datetime(0)) : data_need_id(var_id), start(var_start), end(var_end) {};

		bool is_correct () {return start > datetime(0) and end > datetime(0) and start < end;};
		
		vector<time_period> get_time_periods () {
			return is_correct() ? data_cache<time_period>::get_all(& time_period::start, start, end) : vector<time_period>();
		};
		vector<work_checked> get_work_checkeds () {
			return is_correct() ? data_cache<work_checked>::get_all(& work_checked::check, start, end) : vector<work_checked>();
		};
};

#endif 
