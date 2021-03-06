#include "main_module.h"
#include <iostream>
#include "data_provider.h"
#include "utils.h"

using std::cout;
using std::wcout;
using std::cin;
using std::wcin;
using std::endl;
using std::flush;
using std::string;
using std::wstring;
using utils::wstring_to_int;
using utils::int_to_wstring;
using utils::split_wstring_by_space;
using utils::merge_wstring;
using utils::num_declination;

void main_module :: clear_screen () {
	wcout << flush;
	cout << "\E[H\E[J" << flush;
}

void main_module :: print (const string & val, bool ff, bool need_endl) {
	cout << val;
	if (need_endl) cout << endl;
	if (ff) cout << flush;
}
void main_module :: print (const wstring & val, bool ff, bool need_endl) {
	wcout << val;
	if (need_endl) wcout << endl;
	if (ff) wcout << flush;
}
void main_module :: print (const char * val, bool ff, bool need_endl) {
	cout << val;
	if (need_endl) cout << endl;
	if (ff) cout << flush;
}
void main_module :: print (const wchar_t * val, bool ff, bool need_endl) {
	wcout << val;
	if (need_endl) wcout << endl;
	if (ff) wcout << flush;
}

vector<command> & main_module :: commands = commands_init();

vector<command> & main_module :: commands_init () {
	static vector<command> result;
	result.push_back(command(L"выход", L"Завершение работы программы без закрытия последнего временного периода.", & main_module::command_exit));
	result.push_back(command(L"помощь", L"Наберите \"помощь команда\" для получения справки по команде. Доступны команды помощь, проверить, пров, добавить, список, удалить, скрыть, вернуть, старт, стоп, отметить, выход.", & main_module::command_help));
	result.push_back(command(L"проверить", L"Проверить (обновить) статус и вывести его.", & main_module::command_check_status));
	result.push_back(command(L"пров", L"Псевдоним команды проверить. Проверить (обновить) статус и вывести его.", & main_module::command_check_status));
	result.push_back(command(L"добавить", L"Добавить день с указанием его окончания \"добавить день HH:MM\" (если время указывается раньше текущего момента, считается что оно указано в следующих сутках), добавить однократное дело с указанием количества повторений и периода \"добавить дело одн название количество\", добавить продолжительное дело с указанием нормы минут в день \"добавить дело прод название норма\". Название может состоять из нескольких слов, разделенных пробелами.", & main_module::command_add));
	result.push_back(command(L"обновить", L"Обновить окончание текущего дня \"обновить день HH:MM\", новое окончание дня не может быть раньше текущего момента. Вместо \"HH:MM\" можно написать \"+число\", это количество минут добавится к последнему дню. Обновить дело \"обновить дело id название количество\" или \"обновить дело id название норма\", в зависимости от типа дела. Название может состоять из нескольких слов, разделенных пробелами. Вместо любого параметра можно проставить тире (-), его значение будет сохранено.", & main_module::command_update));
	result.push_back(command(L"список", L"Можно посмотреть список дел, отметок, периодов и учета. Список учета - это список отметок вместе с периодами. Можно посмотреть список скрытых дел командой \"список дел скрытых\". Отметки и периоды выводятся за последний учитываемый день.", & main_module::command_list));
	result.push_back(command(L"скрыть", L"Параметр: id дела. Отмечает дело удаленным, но все данные сохраняются. Если идет его учет, он останавливается.", & main_module::command_hide));
	result.push_back(command(L"вернуть", L"Параметр: id дела. Возвращает скрытое дело в список активных.", & main_module::command_return));
	result.push_back(command(L"старт", L"Параметр: id продолжительного дела. Открывает временной промежуток. Третьим необязательным параметро можно добавить количество минут, уже потраченных на дело. Начало временного промежутка будет соответствующе сдвинуто на более ранне время.", & main_module::command_start));
	result.push_back(command(L"стоп", L"Закрывает текущий открытый временной промежуток. Необязательный второй параметр - сколько минут было потрачено не на дело. Окончание промежутка будет сдвинуто на более ранне время.", & main_module::command_stop));
	result.push_back(command(L"отметить", L"Параметр: id однократного дела. Добавляет отметку о выполнении.", & main_module::command_mark));
	result.push_back(command(L"удалить", L"Параметр: id дела. Безвозвратно удаляет дело и все его отметки/временные периоды.", & main_module::command_delete));
	return result;
}

wstring main_module :: command_delete (const vector<wstring> & params) {
	if (params.size() > 0) {
		int id = wstring_to_int(params[0]);
		if (id > 0) {
			one_work ow = data_cache<one_work>::get_by_id(id);
			long_work lw = data_cache<long_work>::get_by_id(id);
			if (ow.is_valid() or lw.is_valid()) {
				if (ow.is_valid()) dp()->delete_one_work(id);
				else dp()->delete_long_work(id);
				return L"Дело удалено успешно";
			} else {
				return L"Ошибка - дело с указанным id не найдено.";
			}
		} else {
			return L"Ошибка - некорректный id дела.";
		}
	} else {
		return L"Ошибка - не указан id дела.";
	}
}

wstring main_module :: command_mark (const vector<wstring> & params) {
	if (params.size() > 0) {
		int id = wstring_to_int(params[0]);
		if (id > 0) {
			one_work ow = data_cache<one_work>::get_by_id(id);
			if (ow.is_valid()) {
				work_checked element(0, id, datetime());
				dp()->add_work_checked(element);
				return L"Добавлена отметка о выполнении дела " + int_to_wstring(id) + L" (\"" + ow.name + L"\")";
			} else {
				return L"Ошибка - не найдено однократное дело с указанным id.";
			}
		} else {
			return L"Ошибка - некорректный id дела.";
		}
	} else {
		return L"Ошибка - не указан id дела.";
	}
}

wstring main_module :: command_stop (const vector<wstring> & params) {
	// [минуты]
	time_period last = data_cache<time_period>::get_last();
	if (last.is_valid() and last.is_opened()) {
		last.end = datetime();
		if (params.size() >= 1) {
			int minuts = wstring_to_int(params[0]);
			if (minuts > 0) {
				last.end.change_by_seconds(-minuts * 60);
				if (last.start >= last.end) {
					last.end = last.start;
					last.end.change_by_seconds(1);
				}
			}
		}
		dp()->update_time_period(last);
		return L"Учет дела остановлен.";
	} else {
		return L"Ошибка - в текущий момент не ведется учет.";
	}
}

wstring main_module :: command_start (const vector<wstring> & params) {
	// id [минуты]
	if (params.size() >= 1) {
		int id = wstring_to_int(params[0]);
		if (id > 0) {
			long_work lw = data_cache<long_work>::get_by_id(id);
			if (lw.is_valid()) {
				datetime start;
				if (params.size() >= 2) {
					int minuts = wstring_to_int(params[1]);
					if (minuts > 0) start.change_by_seconds(-minuts * 60);
				}
				time_period element(0, id, start, datetime(0));
				dp()->add_time_period(element);
				return L"Начат учет дела " + int_to_wstring(id) + L" (\"" + lw.name + L"\")";
			} else {
				return L"Ошибка - не найдено продолжительное дело с указанным id.";
			}
		} else {
			return L"Ошибка - некорректный id дела.";
		}
	} else {
		return L"Ошибка - не указан id дела.";
	}
}

wstring main_module :: command_update (const vector<wstring> & params) {
	size_t psize = params.size();
	if (psize < 2) {
		return L"Ошибка - недостаточно параметров команды.";
	} else {
		// день HH:MM | день +число
		if (params[0] == L"день") {
			day last = data_cache<day>::get_last();
			if (last.is_valid()) {
				datetime dt;
				if (params[1][0] == L'+') {
					int add_val;
					if (params[1].length() <= 1) {
						return L"Ошибка - не указана добавка к окончанию последнего учитываемого дня.";
					} else if ((add_val = wstring_to_int(params[1].substr(1))) > 0) {
						last.end.change_by_seconds(add_val * 60);
						dp()->update_day(last);
						return L"День успешно обновлен.";
					} else {
						return L"Ошибка - некорректная добавка к окончанию последнего учитываемого дня.";
					}
				} else if (set_day_end_by_HH_MM(params[1], dt)) {
					last.end = dt;
					dp()->update_day(last);
					return L"День успешно обновлен.";
				} else {
					return L"Ошибка - некорректный параметр.";
				}
			} else {
				return L"Ошибка - еще нет учитываемых дней.";
			}
		// дело id название количество | дело id название норма
		} else if (params[0] == L"дело") {
			int id = wstring_to_int(params[1]);
			if (id <= 0) {
				return L"Ошибка - некорректный id дела.";
			} else if (psize < 4) {
				return L"Ошибка - недостаточно параметров команды.";
			} else {
				one_work ow = data_cache<one_work>::get_by_id(id);
				long_work lw = data_cache<long_work>::get_by_id(id);
				wstring name = merge_wstring(params, 2, psize - 2);
				if (ow.is_valid()) {
					if (name != L"-") ow.name = name;
					if (params[psize - 1] != L"-") ow.count = wstring_to_int(params[psize - 1]);
					dp()->update_one_work(ow);
					return L"Однократное дело " + int_to_wstring(id) + L" успешно обновлено.";
				} else if (lw.is_valid()) {
					if (name != L"-") lw.name = name;
					if (params[psize - 1] != L"-") lw.plan = wstring_to_int(params[psize - 1]);
					dp()->update_long_work(lw);
					return L"Продолжительное дело " + int_to_wstring(id) + L" успешно обновлено.";
				} else {
					return L"Ошибка - дело с указанным id не найдено.";
				}
			}
		} else {
			return L"Ошибка - неопознанный параметр команды.";
		}
	}
}

wstring main_module :: command_return (const vector<wstring> & params) {
	if (params.size() > 0) {
		int id = wstring_to_int(params[0]);
		if (id > 0) {
			one_work ow = data_cache<one_work>::get_by_id(id);
			long_work lw = data_cache<long_work>::get_by_id(id);
			if (ow.is_valid() and ow.deleted) {
				ow.deleted = false;
				dp()->update_one_work(ow);
				return L"Дело " + int_to_wstring(id) + L" (\"" + ow.name + L"\")" + L" снова активно.";
			} else if (lw.is_valid() and lw.deleted) {
				lw.deleted = false;
				dp()->update_long_work(lw);
				return L"Дело " + int_to_wstring(id) + L" (\"" + lw.name + L"\")" + L" снова активно.";
			} else {
				return L"Дело с уазанным id не найдено, либо уже активно.";
			}
		} else {
			return L"Ошибка - некорректный параметр команды.";
		}
	} else {
		return L"Ошибка - не указан параметр команды.";
	}
}

wstring main_module :: command_hide (const vector<wstring> & params) {
	if (params.size() > 0) {
		int id = wstring_to_int(params[0]);
		if (id > 0) {
			one_work ow = data_cache<one_work>::get_by_id(id);
			long_work lw = data_cache<long_work>::get_by_id(id);
			if (ow.is_valid() and !ow.deleted) {
				ow.deleted = true;
				dp()->update_one_work(ow);
				return L"Дело " + int_to_wstring(id) + L" (\"" + ow.name + L"\")" + L" успешно скрыто.";
			} else if (lw.is_valid() and !lw.deleted) {
				lw.deleted = true;
				time_period tp = data_cache<time_period>::get_last();
				if (tp.is_valid() and tp.work_id == id and tp.is_opened()) {
					tp.end = datetime();
					dp()->update_time_period(tp);
				}
				dp()->update_long_work(lw);
				return L"Дело " + int_to_wstring(id) + L" (\"" + lw.name + L"\")" + L" успешно скрыто.";
			} else {
				return L"Дело с уазанным id не найдено, либо уже скрыто.";
			}
		} else {
			return L"Ошибка - некорректный параметр команды.";
		}
	} else {
		return L"Ошибка - не указан параметр команды.";
	}
}

wstring main_module :: command_help (const vector<wstring> & params) {
	command com, help = parse_command(L"помощь");
	return (params.size() > 0 and (com = parse_command(params[0])).is_valid()) ? com.help_message : help.help_message;
}

wstring main_module :: command_check_status (const vector<wstring> & params) {
	return L"";
}

bool main_module :: set_day_end_by_HH_MM (const wstring & param, datetime & val) {
	bool result = false;
	time_count d_end;
	if (param.length() >= 5 and d_end.parse_from_HH_MM_SS(param.substr(0, 5) + L":00")) {
		datetime dt;
		tm tinfo = dt.get_timeinfo();
		if (d_end <= dt.get_time_count()) ++tinfo.tm_mday;
		tinfo.tm_sec = 0;
		tinfo.tm_min = d_end.m;
		tinfo.tm_hour = d_end.h;
		val.set(tinfo);
		result = true;
	}
	return result;
}

wstring main_module :: command_add (const vector<wstring> & params) {
	int size = params.size(), new_id;
	if (size < 2) {
		return L"Ошибка - недостаточно параметов команды.";
	} else {
		if (params[0] == L"день") {
			// день HH:MM
			datetime dt;
			if (set_day_end_by_HH_MM(params[1], dt)) {
				day element(0, datetime(), dt);
				new_id = dp()->add_day(element);
				return L"Новый день добавлен успешно.";
			} else {
				return L"Ошибка - некорректный параметр.";
			}
		} else if (params[0] == L"дело") {
			// дело одн название количество | дело прод название норма
			if (size >= 4) {
				int num = wstring_to_int(params[size - 1]);
				if (num > 0) {
					wstring name = merge_wstring(params, 2, size - 2);
					if (params[1] == L"одн") {
						new_id = dp()->add_one_work(one_work(0, name, num, datetime(), false));
						return L"Однократное дело \"" + name + L"\" успешно добавлено с id = " + int_to_wstring(new_id);
					} else if (params[1] == L"прод") {
						new_id = dp()->add_long_work(long_work(0, name, num, datetime(), false));
						return L"Продолжительное дело \"" + name + L"\" успешно добавлено с id = " + int_to_wstring(new_id);
					} else {
						return L"Ошибка - некорректные параметры команды.";
					}
				} else {
					return L"Ошибка - некорректные параметры команды.";
				}
			} else {
				return L"Ошибка - недостаточно параметров команды.";
			}
		} else {
			return L"Ошибка - не опознанные параметры.";
		}
	}
}

template<class C>
wstring main_module :: get_wstring_from_list (const wchar_t * what, const vector<C> & data, const wchar_t * err) {
	size_t size = data.size();
	wstring result(L"");
	if (size > 0) {
		result += what;
		result += L" (всего " + int_to_wstring(size) + L"):\n";
		for (typename vector<C>::const_iterator i = data.begin(); i != data.end(); ++i) result += L"    " + i->get_wstring() + L"\n";
	} else {
		result += err;
		result += L"\n";
	}
	return result;
}

wstring main_module :: command_list (const vector<wstring> & params) {
	if (params.size() > 0) {
		// дел | отметок | периодов | учета
		if (params[0] == L"дел") {
			wstring result(L"");
			bool need_hidden = (params.size() > 1 and params[1] == L"скрытых");
			result += get_wstring_from_list((need_hidden ? L"Скрытые однократные дела" : L"Однократные дела"), data_cache<one_work>::get_all<bool>(& one_work::deleted, need_hidden), (need_hidden ? L"Нет скрытых однократных дел." : L"Однократные дела не найдены."));
			result += get_wstring_from_list((need_hidden ? L"Скрытые продолжительные дела" : L"Продолжительные дела"), data_cache<long_work>::get_all<bool>(& long_work::deleted, need_hidden), (need_hidden ? L"Нет скрытых продолжительных дел." : L"Продолжительные дела не найдены."));
			return result;
		} else if (params[0] == L"отметок" or params[0] == L"периодов" or params[0] == L"учета") {
			day last_day = data_cache<day>::get_last();
			if (last_day.is_valid()) {
				wstring result(L"");
				if (params[0] == L"отметок" or params[0] == L"учета") result += get_wstring_from_list(L"Отметки о выполнении однократных дел", last_day.get_work_checkeds(), L"В последнем учитываемом дне не найдено отметок о выполнении однократных дел.");
				if (params[0] == L"периодов" or params[0] == L"учета") result += get_wstring_from_list(L"Временные периоды", last_day.get_time_periods(), L"В последнем учитываемом дне не найдены временные периоды.");
				return result;
			} else {
				return L"Нет данных для вывода, так как еще нет учитываемых дней.";
			}
		} else {
			return L"Ошибка - не опознаный параметр.";
		}
	} else {
		return L"Ошибка - не указан параметр команды.";
	}
}

wstring main_module :: command_exit (const vector<wstring> & params) {
	clear_screen();
	exit(0);
}

void main_module :: check_last_time_period_for_close () {
	time_period last_period = data_cache<time_period>::get_last();
	if (last_period.is_valid() and last_period.is_opened()) {
		day last_day = data_cache<day>::get_last();
		if (!last_day.is_valid()) throw storage_query_error(0, L"Ошибка БД - потеряны данные о учитываемых днях");
		if (last_day.end <= datetime()) {
			last_period.end = last_day.end;
			dp()->update_time_period(last_period);
		}
	}
}

template<class W> map <int, wstring> main_module :: get_names_map (const vector<W> & val) {
	map <int, wstring> result;
	for (typename vector<W>::const_iterator i = val.begin(); i != val.end(); ++i) result[i->id] = i->name;
	return result;
}

void main_module :: print_status () {
	size_t days_count = data_cache<day>::size();
	day last_day = data_cache<day>::get_last();
	if (last_day.is_valid() and last_day.end > datetime()) {
		print(L"Учитывается день: " + last_day.start.w_get_print_format() + L" - " + last_day.end.w_get_print_format());
		vector<time_period> day_time_periods = last_day.get_time_periods();
		size_t tp_count = day_time_periods.size();
		if (tp_count > 0 and day_time_periods.back().is_opened()) {
			long_work lw = day_time_periods.back().get_work();
			print(L"Ведется учет дела \"" + lw.name + L"\" (id " + int_to_wstring(lw.id) + L")");
		} else {
			print(L"На текущий момент не ведется учет продолжительного дела.");
		}
		vector<work_checked> day_work_checkeds = last_day.get_work_checkeds();
		print(L"");
		map<int,int> need_one_work_checkeds, need_long_work_minuts;
		vector<one_work> one_works = data_cache<one_work>::get_all<bool>(& one_work::deleted, false);
		for (vector<one_work>::const_iterator i = one_works.begin(); i != one_works.end(); ++i) need_one_work_checkeds[i->id] = i->count;
		for (vector<work_checked>::const_iterator i = day_work_checkeds.begin(); i != day_work_checkeds.end(); ++i) --need_one_work_checkeds[i->work_id];
		vector<long_work> long_works = data_cache<long_work>::get_all<bool>(& long_work::deleted, false);
		for (vector<long_work>::const_iterator i = long_works.begin(); i != long_works.end(); ++i) need_long_work_minuts[i->id] = i->plan * 60;
		for (vector<time_period>::const_iterator i = day_time_periods.begin(); i != day_time_periods.end(); ++i)
			need_long_work_minuts[i->work_id] -= ((i->is_opened() ? datetime() : i->end) - i->start).get_seconds();
		bool ow_title = false, lw_title = false;
		map <int, wstring> ow_names = get_names_map(one_works), lw_names = get_names_map(long_works);
		for (map<int,int>::const_iterator i = need_one_work_checkeds.begin(); i != need_one_work_checkeds.end(); ++i)
			if (i->second > 0) {
				if (!ow_title) {
					print(L"Осталось сделать однократных дел:");
					ow_title = true;
				}
				print(L"    " + int_to_wstring(i->second) + num_declination(i->second, L" раз - ", L" раза - ", L" раз - ") + ow_names[i->first] + L"   [id " + int_to_wstring(i->first) + L"]");
			}
		long int all_need_seconds = 0;
		for (map<int,int>::const_iterator i = need_long_work_minuts.begin(); i != need_long_work_minuts.end(); ++i)
			if (i->second > 0) {
				if (!lw_title) {
					print(L"Осталось запланировано продолжительных дел:");
					lw_title = true;
				}
				all_need_seconds += i->second;
				print(L"   " + time_count(i->second).get_wstr_for_print('w') + L" - " + lw_names[i->first] + L"   [id " + int_to_wstring(i->first) + L"]");
			}
		datetime now;
		time_count from_day_end = last_day.end - now;
		print(L"До окончания учитываемого дня:     " + from_day_end.get_wstr_for_print());
		time_count all_free_time = from_day_end - time_count(all_need_seconds);
		print(L"Всего осталось свободного времени: " + all_free_time.get_wstr_for_print());
		if (all_free_time.get_seconds() > 0) {
			long int from_day_end_seconds = from_day_end.get_seconds();
			if (from_day_end_seconds != 0) {
				int free_minuts_per_hour = (60 * all_free_time.get_seconds()) / from_day_end_seconds; // При больших числах с осторожностью.
				print(L"Свободных минут на каждый час до окончания дня: " + int_to_wstring(free_minuts_per_hour));
			}
		}
	} else {
		print(days_count > 0 ? L"Последний учитываемый день уже завершен." : L"Еще нет учитываемых дней.");
	}
	print(L"\nДля помощи наберите команду помощь.", true);
}

command main_module :: parse_command (const wstring & val) {
	for (vector<command>::const_iterator i = commands.begin(); i != commands.end(); ++i)
		if (i->str == val) return *i;
	return command();
}

void main_module :: start () {
	wstring command_result(L""), command_line;
	vector<wstring> splitted_line;
	command selected_command;
	while (true) {
		command_line = L"";
		splitted_line.clear();
		clear_screen();
		if (command_result.length() > 0) print(command_result + L'\n');
		check_last_time_period_for_close();
		print_status();
		dp()->free();
		print(L"\nВведите команду: ", true, false);
		getline(wcin, command_line);
		splitted_line = split_wstring_by_space(command_line);
		if (splitted_line.size() == 0) splitted_line.push_back(L"пров");
		selected_command = parse_command(splitted_line[0]);
		if (selected_command.is_valid()) {
			splitted_line.erase(splitted_line.begin());
			try {
				command_result = selected_command.fun(splitted_line);
			} catch (storage_operations & exp) {
				command_result = exp.message;
			}
		} else {
			command_result = L"Ошибка - неопознанная команда.";
		}
	}
	
}
