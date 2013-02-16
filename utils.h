#ifndef _UTILS_
#define _UTILS_

#include <string>
#include <map>
#include <vector>

using std::string;
using std::wstring;
using std::map;
using std::vector;

// Место для всяких штук, которые не подходят ни в одно другое место.

namespace utils {

	// Преобразование string <=> wstring сделано простым примитивным способом, только для русского, и на другом компьютере может не сработать. Но пока не разобрался с более сложным.

	class char_pair {
		public:
			char first;
			char second;
			short int len;

			char_pair (const string & val);
			bool operator == (const char_pair & val) const;
			bool operator < (const char_pair & val) const;
			bool check_match (const string & val, size_t n) const;
	};

	class strmaps {
		private:
			static map <wchar_t, string> & w2s_init ();
			static map <char_pair, wchar_t> & s2w_init ();
		public:
			static const map <wchar_t, string> & w2s;
			static const map <char_pair, wchar_t> & s2w;
	};

	string int_to_string (int);
	wstring int_to_wstring (int);
	int string_to_int (const string &); // Если в строке не целое число, то возвращается 0.
	int wstring_to_int (const wstring &); // Если в строке не целое число, то возвращается 0.
	string wstr2str (const wstring &);
	wstring str2wstr (const string &);
	wstring escape_double_quotes (wstring);
	vector<wstring> split_wstring_by_space (const wstring &);
	wstring merge_wstring (const vector<wstring> &, size_t = 0, size_t = wstring::npos, const wstring & = L" "); // Склейка вектора из сток в одну строку, включая начальную и конечную позиции. Между подстроками вставляется 4-ый параметр.
	const wchar_t * num_declination (unsigned int, const wchar_t *, const wchar_t *, const wchar_t *); // Возвращает нужную форму склонения в зависимости от числа (например, день дня дней). 
}

#endif 
