#include "content_types.h"

#include "utils.h"
using utils::str2wstr;
using utils::wstr2str;
using utils::int_to_wstring;
using utils::num_declination;

#include <iostream>
using std::cout;
using std::wcout;
using std::endl;
using std::flush;

void time_count :: init_from_seconds (long int seconds) {
	h = m = s = 0;
	if (seconds != 0) {
		bool flag_minus = seconds < 0; // Если операнд меньше 0, то поведение операции % зависит от реализации. Выберем надежный путь. 
		if (flag_minus) seconds = -seconds;
		seconds -= (s = seconds % 60);
		if (seconds > 0) {
			seconds /= 60;
			seconds -= (m = seconds % 60);
			if (seconds > 0) h = seconds / 60;
		}
		if (flag_minus) {
			h = -h;
			m = -m;
			s = -s;
		}
	}
}

bool time_count :: parse_from_HH_MM_SS (const string & val) {
	bool result = false;
	if (
		val.size() >= 8 and
		isdigit(val[0]) and isdigit(val[1]) and
		isdigit(val[3]) and isdigit(val[4]) and
		isdigit(val[6]) and isdigit(val[7])
	) {
		h = atoi(val.substr(0, 2).c_str());
		m = atoi(val.substr(3, 2).c_str());
		s = atoi(val.substr(6, 2).c_str());
		result = true;
	}
	return result;
}

long int time_count :: get_seconds () const {
	return h * 3600 + m * 60 + s;
}

bool time_count :: parse_from_HH_MM_SS (const wstring & val) {
	return parse_from_HH_MM_SS(wstr2str(val));
}

time_count :: time_count (long int seconds) : h(0), m(0), s(0) {
	init_from_seconds(seconds);
}

time_count :: time_count (long int vh, long int vm, long int vs) : h(0), m(0), s(0) {
	init_from_seconds (vh * 3600 + vm * 60 + vs); // Вместо прямой инициации сделано так, что бы можно было задавать параметры с разным знаком, минуты или секунды больше 60 или меньше -60, и в результате получить корректный объект. 
};

wstring time_count :: get_wstr_for_print (char type) const {
	wstring result(L"");
	if (type == 'w') {
		// Как нормальный вывод, но если первые значения нулевые, их место заполняется пробелами. Так же добавляется пробел к значениям меньшим 10.
		if (h == 0) {
			if (m == 0) result = wstring(L"          ") + (s < 10 ? L" " : L"") + int_to_wstring(s) + L"с.";
			else result = wstring(L"     ") + (m < 10 ? L" " : L"") + int_to_wstring(m) + L"м. " + (s < 10 ? L" " : L"") + int_to_wstring(s) + L"с.";
		} else {
			result = (h < 10 ? L" " : L"") + int_to_wstring(h) + L"ч. " + (m < 10 ? L" " : L"") + int_to_wstring(m) + L"м. " + (s < 10 ? L" " : L"") + int_to_wstring(s) + L"с.";
		}
	} else if (type == 'c') {
		// Показываются только ненулевые значения.
		if (h != 0) result += int_to_wstring(h) + L"ч." + (m != 0 or s != 0 ? L" " : L"");
		if (m != 0) result += int_to_wstring(m) + L"м." + (s != 0 ? L" " : L"");
		if (s != 0 or (h == 0 and m == 0 and s == 0)) result += int_to_wstring(s) + L"с.";
	} else {
		// Нулевые значения показываются, если перед ними был вывод.
		if (h == 0) {
			if (m == 0) result = int_to_wstring(s) + L"с.";
			else result = int_to_wstring(m) + L"м. " + int_to_wstring(s) + L"с.";
		} else {
			result = int_to_wstring(h) + L"ч. " + int_to_wstring(m) + L"м. " + int_to_wstring(s) + L"с.";
		}
	}
	return result;
};

time_count time_count :: operator - (const time_count & val) const {
	return time_count(get_seconds() - val.get_seconds());
}

string datetime :: get_sqlite_format () const {
	if (seconds == 0) {
		return string("");
	} else {
		char str[20];
		strftime(str, 20, "%Y-%m-%d %H:%M:%S", & timeinfo);
		return string(str);
	}
}

string datetime :: get_print_format (bool need_e) const {
	if (seconds == 0) {
		return string("---");
	} else {
		char str[17];
		if (need_e) strftime(str, 17, "%d-е %H:%M:%S", & timeinfo);
		else strftime(str, 17, "%d-го %H:%M:%S", & timeinfo);
		string result(str);
		if (result[0] == '0') result.erase(0, 1);
		return result;
	}
}

wstring datetime :: w_get_print_format (bool need_e) const {
	return str2wstr(get_print_format(need_e));
}

bool datetime :: parse_sqlite_format (string value) {
	bool result = false;
	if (
		value.length() >= 19 and
		isdigit(value[0]) and isdigit(value[1]) and isdigit(value[2]) and isdigit(value[3]) and value[4] == '-' and 	/* год */
		isdigit(value[5]) and isdigit(value[6]) and value[7] == '-' and 	/* месяц */
		isdigit(value[8]) and isdigit(value[9]) and value[10] == ' ' and 	/* день */
		isdigit(value[11]) and isdigit(value[12]) and value[13] == ':' and 	/* часы */
		isdigit(value[14]) and isdigit(value[15]) and value[16] == ':' and 	/* минуты */
		isdigit(value[17]) and isdigit(value[18]) 							/* секунды */
	) {
		tm new_tm;
		new_tm.tm_year = atoi(value.substr(0, 4).c_str()) - 1900;
		new_tm.tm_mon = atoi(value.substr(5, 2).c_str()) - 1;
		new_tm.tm_mday = atoi(value.substr(8, 2).c_str());
		new_tm.tm_hour = atoi(value.substr(11, 2).c_str());
		new_tm.tm_min = atoi(value.substr(14, 2).c_str());
		new_tm.tm_sec = atoi(value.substr(17, 2).c_str());
		new_tm.tm_isdst = -1;
		// Границы входных данных не проверяем, так как данные будут получены из БД, в которую будет писать только сама программа.
		seconds = mktime(& new_tm);
		timeinfo = * localtime(& seconds);
		result = true;
	}
	return result;
};

void datetime :: set (time_t val) {
	seconds = val;
	timeinfo = * localtime(& seconds);
}

void datetime :: set (tm val) {
	seconds = mktime(& val);
	timeinfo = * localtime(& seconds);
}

void datetime :: test_print () const {
	cout << flush;
	wcout << flush;
	wcout << L"Строка в формате БД SQLite: " << flush;
	cout << get_sqlite_format() << endl << flush;
	wcout << L"Всего секунд с начала отсчета: " << seconds << endl;
	wcout << L"Разбор структуры tm: " << endl;
	wcout << L"    Количество лет с 1900: " << timeinfo.tm_year << endl;
	wcout << L"    Количество месяцев с января: " << timeinfo.tm_mon << endl;
	wcout << L"    День месяца: " << timeinfo.tm_mday << endl;
	wcout << L"    Количество часов: " << timeinfo.tm_hour << endl;
	wcout << L"    Количество минут: " << timeinfo.tm_min << endl;
	wcout << L"    Количество секунд: " << timeinfo.tm_sec << endl;
	wcout << L"    Количество дней с воскресенья: " << timeinfo.tm_wday << endl;
	wcout << L"    Количество дней с 1 января: " << timeinfo.tm_yday << endl;
	wcout << L"    Флаг перехода на летнее время: " << timeinfo.tm_isdst << endl << endl << flush;
}

void datetime :: change_by_seconds (long int sec) {
	time_t correct_value = (sec < 0 ? -sec : sec);
	if (sec < 0) seconds -= (correct_value > seconds ? seconds : correct_value); // С беззнаковыми типами нужно обращаться с осторожностью.
	else seconds += correct_value;
	timeinfo = *localtime(& seconds);
}

wstring data_need_id :: get_wstring () const {
	return int_to_wstring(id);
};

wstring work :: get_wstring () const {
	return data_need_id::get_wstring() + L" \"" + name + L"\" от " + str2wstr(created.get_sqlite_format());
};

wstring one_work :: get_wstring () const {
	return work::get_wstring() + L" (" + int_to_wstring(count) + num_declination(count, L" раз в день)", L" раза в день)", L" раз в день)");
};

wstring long_work :: get_wstring () const {
	//return work::get_wstring() + L" (" + int_to_wstring(plan) + num_declination(plan, L" минута в день)", L" минуты в день)", L" минут в день)");
	return work::get_wstring() + L" (" + time_count(plan * 60).get_wstr_for_print('c') + L" в день)";
};

wstring time_period :: get_wstring () const {
	return L"Дело " + int_to_wstring(work_id) + L" (\"" + data_cache<long_work>::get_by_id(work_id).name + L"\") выполнялось с " + start.w_get_print_format() + L" по " + (end.is_zero() ? L"текущий момент" : end.w_get_print_format(true));
};

wstring work_checked :: get_wstring () const {
	return L"Дело " + int_to_wstring(work_id) + L" (\"" + data_cache<one_work>::get_by_id(work_id).name + L"\") выполнено " + check.w_get_print_format();
};
