#include "utils.h"
#include <cstdio>
#include <cstdlib>

namespace utils {

	char_pair :: char_pair (const string & val) : first(0), second(0), len(0) {
		size_t val_len = val.length();
		if (val_len == 1) {
			second = val[0];
			len = 1;
		} else if (val_len > 1) {
			first = val[0];
			second = val[1];
			len = 2;
		}
	};
	bool char_pair :: operator == (const char_pair & val) const {
		return len == val.len and first == val.first and second == val.second;
	};
	bool char_pair :: operator < (const char_pair & val) const {
		return first < val.first or (first == val.first and second < val.second);
	};
	bool char_pair :: check_match (const string & val, size_t n) const {
		size_t val_len = val.length();
		return (len == 1 and n < val_len and first == 0 and second == val[n])
			or (len == 2 and (n + 1) < val_len and first == val[n] and second == val[n + 1]);
	};

	map <wchar_t, string> & strmaps :: w2s_init () {
		static map <wchar_t, string> w2stab;
		w2stab[L'А'] = string("А");
		w2stab[L'Б'] = string("Б");
		w2stab[L'В'] = string("В");
		w2stab[L'Г'] = string("Г");
		w2stab[L'Д'] = string("Д");
		w2stab[L'Е'] = string("Е");
		w2stab[L'Ё'] = string("Ё");
		w2stab[L'Ж'] = string("Ж");
		w2stab[L'З'] = string("З");
		w2stab[L'И'] = string("И");
		w2stab[L'Й'] = string("Й");
		w2stab[L'К'] = string("К");
		w2stab[L'Л'] = string("Л");
		w2stab[L'М'] = string("М");
		w2stab[L'Н'] = string("Н");
		w2stab[L'О'] = string("О");
		w2stab[L'П'] = string("П");
		w2stab[L'Р'] = string("Р");
		w2stab[L'С'] = string("С");
		w2stab[L'Т'] = string("Т");
		w2stab[L'У'] = string("У");
		w2stab[L'Ф'] = string("Ф");
		w2stab[L'Х'] = string("Х");
		w2stab[L'Ц'] = string("Ц");
		w2stab[L'Ч'] = string("Ч");
		w2stab[L'Ш'] = string("Ш");
		w2stab[L'Щ'] = string("Щ");
		w2stab[L'Ъ'] = string("Ъ");
		w2stab[L'Ы'] = string("Ы");
		w2stab[L'Ь'] = string("Ь");
		w2stab[L'Э'] = string("Э");
		w2stab[L'Ю'] = string("Ю");
		w2stab[L'Я'] = string("Я");
		w2stab[L'а'] = string("а");
		w2stab[L'б'] = string("б");
		w2stab[L'в'] = string("в");
		w2stab[L'г'] = string("г");
		w2stab[L'д'] = string("д");
		w2stab[L'е'] = string("е");
		w2stab[L'ё'] = string("ё");
		w2stab[L'ж'] = string("ж");
		w2stab[L'з'] = string("з");
		w2stab[L'и'] = string("и");
		w2stab[L'й'] = string("й");
		w2stab[L'к'] = string("к");
		w2stab[L'л'] = string("л");
		w2stab[L'м'] = string("м");
		w2stab[L'н'] = string("н");
		w2stab[L'о'] = string("о");
		w2stab[L'п'] = string("п");
		w2stab[L'р'] = string("р");
		w2stab[L'с'] = string("с");
		w2stab[L'т'] = string("т");
		w2stab[L'у'] = string("у");
		w2stab[L'ф'] = string("ф");
		w2stab[L'х'] = string("х");
		w2stab[L'ц'] = string("ц");
		w2stab[L'ч'] = string("ч");
		w2stab[L'ш'] = string("ш");
		w2stab[L'щ'] = string("щ");
		w2stab[L'ъ'] = string("ъ");
		w2stab[L'ы'] = string("ы");
		w2stab[L'ь'] = string("ь");
		w2stab[L'э'] = string("э");
		w2stab[L'ю'] = string("ю");
		w2stab[L'я'] = string("я");
		return w2stab;
	}

	map <char_pair, wchar_t> & strmaps :: s2w_init () {
		static map <char_pair, wchar_t> s2wtab;
		s2wtab[string("А")] = L'А';
		s2wtab[string("Б")] = L'Б';
		s2wtab[string("В")] = L'В';
		s2wtab[string("Г")] = L'Г';
		s2wtab[string("Д")] = L'Д';
		s2wtab[string("Е")] = L'Е';
		s2wtab[string("Ё")] = L'Ё';
		s2wtab[string("Ж")] = L'Ж';
		s2wtab[string("З")] = L'З';
		s2wtab[string("И")] = L'И';
		s2wtab[string("Й")] = L'Й';
		s2wtab[string("К")] = L'К';
		s2wtab[string("Л")] = L'Л';
		s2wtab[string("М")] = L'М';
		s2wtab[string("Н")] = L'Н';
		s2wtab[string("О")] = L'О';
		s2wtab[string("П")] = L'П';
		s2wtab[string("Р")] = L'Р';
		s2wtab[string("С")] = L'С';
		s2wtab[string("Т")] = L'Т';
		s2wtab[string("У")] = L'У';
		s2wtab[string("Ф")] = L'Ф';
		s2wtab[string("Х")] = L'Х';
		s2wtab[string("Ц")] = L'Ц';
		s2wtab[string("Ч")] = L'Ч';
		s2wtab[string("Ш")] = L'Ш';
		s2wtab[string("Щ")] = L'Щ';
		s2wtab[string("Ъ")] = L'Ъ';
		s2wtab[string("Ы")] = L'Ы';
		s2wtab[string("Ь")] = L'Ь';
		s2wtab[string("Э")] = L'Э';
		s2wtab[string("Ю")] = L'Ю';
		s2wtab[string("Я")] = L'Я';
		s2wtab[string("а")] = L'а';
		s2wtab[string("б")] = L'б';
		s2wtab[string("в")] = L'в';
		s2wtab[string("г")] = L'г';
		s2wtab[string("д")] = L'д';
		s2wtab[string("е")] = L'е';
		s2wtab[string("ё")] = L'ё';
		s2wtab[string("ж")] = L'ж';
		s2wtab[string("з")] = L'з';
		s2wtab[string("и")] = L'и';
		s2wtab[string("й")] = L'й';
		s2wtab[string("к")] = L'к';
		s2wtab[string("л")] = L'л';
		s2wtab[string("м")] = L'м';
		s2wtab[string("н")] = L'н';
		s2wtab[string("о")] = L'о';
		s2wtab[string("п")] = L'п';
		s2wtab[string("р")] = L'р';
		s2wtab[string("с")] = L'с';
		s2wtab[string("т")] = L'т';
		s2wtab[string("у")] = L'у';
		s2wtab[string("ф")] = L'ф';
		s2wtab[string("х")] = L'х';
		s2wtab[string("ц")] = L'ц';
		s2wtab[string("ч")] = L'ч';
		s2wtab[string("ш")] = L'ш';
		s2wtab[string("щ")] = L'щ';
		s2wtab[string("ъ")] = L'ъ';
		s2wtab[string("ы")] = L'ы';
		s2wtab[string("ь")] = L'ь';
		s2wtab[string("э")] = L'э';
		s2wtab[string("ю")] = L'ю';
		s2wtab[string("я")] = L'я';
		return s2wtab;
	}

	const map <wchar_t, string> & strmaps :: w2s = w2s_init();
	const map <char_pair, wchar_t> & strmaps :: s2w = s2w_init();

	string int_to_string (int val) {
		char buf[21]; // Для 8-байтового int. При увеличении размера int добавить размера в массив. 
		sprintf(buf, "%d", val);
		return string(buf);
	};

	wstring int_to_wstring (int val) {
		return str2wstr(int_to_string(val));
	};

	int string_to_int (const string & val) {
		int result = atoi(val.c_str());
		string check = int_to_string(result);
		if (check != val) result = 0;
		return result;
	};

	int wstring_to_int (const wstring & val) {
		return string_to_int(wstr2str(val));
	};

	string wstr2str (const wstring & str) {
		string result;
		size_t i, len = str.length();
		map <wchar_t, string> :: const_iterator s, end = strmaps::w2s.end();
		for (i = 0; i < len; ++i) {
			s = strmaps::w2s.find(str[i]);
			if (s == end) result += static_cast<char>(str[i]);
			else result += s->second;
		}
		return result;
	};

	wstring str2wstr (const string & val) {
		wstring result;
		size_t i, len = val.length();
		map <char_pair, wchar_t> :: const_iterator tab_i, tab_begin = strmaps::s2w.begin(), tab_end = strmaps::s2w.end();
		bool was_finded;
		for (i = 0; i < len; ++i) {
			was_finded = false;
			for (tab_i = tab_begin; tab_i != tab_end; ++tab_i)
				if (tab_i->first.check_match(val, i)) {
					result += tab_i->second;
					if (tab_i->first.len == 2) ++i;
					was_finded = true;
					break;
				}
			if (!was_finded) result += static_cast<wchar_t>(val[i]);
		}
		return result;
	};

	wstring escape_double_quotes (wstring val) {
		size_t n = 0;
		while (true) {
			n = val.find(L'"', n);
			if (n == wstring::npos) break;
			val.insert(val.begin() + n, L'\\');
			n += 2;
		};
		return val;
	};

	vector<wstring> split_wstring_by_space (const wstring & val) {
		vector<wstring> result;
		bool is_in = false, is_space;
		wstring buff(L"");
		for (wstring::const_iterator i = val.begin(); i != val.end(); ++i) {
			is_space = (*i == L' ' or *i == L'\t');
			if (!is_in and !is_space) {
				is_in = true;
				buff.erase();
				buff.push_back(*i);
			} else if (is_in and !is_space) {
				buff.push_back(*i);
			} else if (is_in and is_space) {
				is_in = false;
				result.push_back(buff);
			}
		}
		if (is_in) result.push_back(buff);
		return result;
	};

	wstring merge_wstring (const vector<wstring> & val, size_t start, size_t end, const wstring & glue) {
		wstring result(L"");
		size_t i, size = val.size();
		bool use_glue = glue.size() > 0;
		if (start <= end and start < size)
			for (i = start; i <= end and i < size; ++i) {
				result += val[i];
				if (use_glue and i < end and i < (size - 1)) result += glue;
			}
		return result;
	};

	const wchar_t * num_declination (unsigned int val, const wchar_t * f1, const wchar_t * f2, const wchar_t * f3) {
		unsigned short int n1 = val % 100, n2 = val % 10;
		const wchar_t * result;
		if (n1 >= 10 and n1 <= 20) result = f3;
		else if (n2 >= 2 and n2 <= 4) result = f2;
		else if (n2 == 1) result = f1;
		else result = f3;
		return result;
	};
}
