#include "data_provider.h"
#include <cstdlib>
#include <algorithm>

#include "utils.h"
using utils::int_to_string;
using utils::str2wstr;
using utils::wstr2str;
using utils::escape_double_quotes;

// created устанавливается на текущий момент, deleted устанавливается в false, остальные данные должны быть корректные. name не должно совпадать с уже имеющимися делами обоих типов.
template <class workC> void data_provider :: check_allow_add_work (workC & element) throw (add_to_storage_error) {
	element.created = datetime();
	element.deleted = false;
	if (!element.is_correct()) throw add_to_storage_error(L"Ошибка добавления дела - некорректные данные для добавления");
	int new_id = 0;
	for (vector<one_work>::const_iterator i = data_cache<one_work>::list.begin(); i != data_cache<one_work>::list.end(); ++i) {
		if (i->name == element.name) throw add_to_storage_error(L"Ошибка добавления дела - уже существует однократное дело (возможно, скрытое) с таким же названием");
		if (i->id > new_id) new_id = i->id;
	}
	for (vector<long_work>::const_iterator i = data_cache<long_work>::list.begin(); i != data_cache<long_work>::list.end(); ++i) {
		if (i->name == element.name) throw add_to_storage_error(L"Ошибка добавления дела - уже существует продолжительное дело (возможно, скрытое) с таким же названием");
		if (i->id > new_id) new_id = i->id;
	}
	element.id = new_id + 1;
}

void data_provider :: check_allow_add_one_work (one_work & element) throw (add_to_storage_error) {
	check_allow_add_work(element);
}
void data_provider :: check_allow_add_long_work (long_work & element) throw (add_to_storage_error) {
	check_allow_add_work(element);
}

// День можно добавлять, только если предыдущий день уже завершился. Начало дня устанавливается на текущий момент. Окончание дня должно быть после начала. 
void data_provider :: check_allow_add_day (day & element) throw (add_to_storage_error) {
	day last_day = data_cache<day>::get_last();
	datetime now;
	if (last_day.is_valid() and last_day.end >= now) throw add_to_storage_error(L"Ошибка добавления дня - текущий день еще не завершен");
	element.start = now;
	if (!element.is_correct()) throw add_to_storage_error(L"Ошибка добавления дня - некорректные данные для добавления");
	element.id = data_cache<day>::get_max_id() + 1;
}

void data_provider :: check_in_day_and_last_period_closed (const wstring err_msg_begin, const datetime & now) throw (add_to_storage_error) {
	day last_day = data_cache<day>::get_last();
	if (!last_day.is_valid()) throw add_to_storage_error(err_msg_begin + L" - еще нет учитываемых дней");
	if (last_day.end <= now) throw add_to_storage_error(err_msg_begin + L" - последний учитываемый день уже завершен");
	time_period last_period = data_cache<time_period>::get_last();
	if (last_period.is_valid() and last_period.is_opened()) throw add_to_storage_error(err_msg_begin + L" - последний период еще не закрыт");
}

// Можно добавлять, только если предыдущий временной период закрыт и текущий день еще не завершен. Дело, к которому привязан период, должно существовать. Начало периода устанавливается на текущий момент, окончание - на datetime(0). 
void data_provider :: check_allow_add_time_period (time_period & element) throw (add_to_storage_error) {
	datetime now;
	check_in_day_and_last_period_closed(L"Ошибка добавления временного периода", now);
	long_work link_work = data_cache<long_work>::get_by_id(element.work_id);
	if (!link_work.is_valid() or link_work.deleted) throw add_to_storage_error(L"Ошибка добавления временного периода - привязанное дело не существует либо скрыто");
	element.start = now;
	element.end = datetime(0);
	if (!element.is_correct()) throw add_to_storage_error(L"Ошибка добавления временного периода - некорректные данные для добавления");
	element.id = data_cache<time_period>::get_max_id() + 1;
}

// Текущий учитываемый день не должен быть завершен. Последний временной период должен быть закрыт. Дело, к которому привязана отметка, должно существовать. Дата-время отметки устанавливается на текущий момент. 
void data_provider :: check_allow_add_work_checked (work_checked & element) throw (add_to_storage_error) {
	datetime now;
	check_in_day_and_last_period_closed(L"Ошибка добавления отметки однократного дела", now);
	one_work link_work = data_cache<one_work>::get_by_id(element.work_id);
	if (!link_work.is_valid() or link_work.deleted) throw add_to_storage_error(L"Ошибка добавления отметки однократного дела - привязанное дело не существует либо скрыто");
	element.check = now;
	if (!element.is_correct()) throw add_to_storage_error(L"Ошибка добавления отметки однократного дела - некорректные данные для добавления");
	element.id = data_cache<work_checked>::get_max_id() + 1;
}

// Элемент с указанным id должен существовать. Дату создания нельзя именить. Данные должны быть корректны. Если меняется название, оно не должно совпадать с уже существующими делами. 
template <class workC> void data_provider :: check_allow_update_work (workC & element) throw (update_storage_error) {
	workC selected = data_cache<workC>::get_by_id(element.id);
	if (!selected.is_valid()) throw update_storage_error(L"Ошибка при обновлении дела - дело с указанным id не найдено.");
	element.created = selected.created;
	if (!element.is_correct()) throw update_storage_error(L"Ошибка обновления дела - некорректные данные для обновления");
	if (element.name != selected.name) {
		for (vector<one_work>::const_iterator i = data_cache<one_work>::list.begin(); i != data_cache<one_work>::list.end(); ++i)
			if (element.id != i->id and element.name == i->name) throw update_storage_error(L"Ошибка обновления дела - другое однократное дело (возможно, скрытое) имеет такое же название");
		for (vector<long_work>::const_iterator i = data_cache<long_work>::list.begin(); i != data_cache<long_work>::list.end(); ++i)
			if (element.id != i->id and element.name == i->name) throw update_storage_error(L"Ошибка обновления дела - другое продолжительное дело (возможно, скрытое) имеет такое же название");
	}
}

void data_provider :: check_allow_update_one_work (one_work & element) throw (update_storage_error) {
	check_allow_update_work(element);
}
void data_provider :: check_allow_update_long_work (long_work & element) throw (update_storage_error) {
	check_allow_update_work(element);
}

// Можно менять только последний учитываемый день. Меняется только окончание дня, причем новое окончание должно быть после текущего момента. 
void data_provider :: check_allow_update_day (day & element) throw (update_storage_error) {
	day last = data_cache<day>::get_last();
	if (!last.is_valid() or element.id != last.id) throw update_storage_error(L"Ошибка при обновлении дня - можно обновить только последний день.");
	if (element.end <= datetime()) throw update_storage_error(L"Ошибка обновления дня - новое окончание для должно быть в будущем");
	element.start = last.start;
	if (!element.is_correct()) throw update_storage_error(L"Ошибка обновления дня - некорректные данные для обновления");
}

// Элемент с указанным id должен существовать. Временной период можно только закрыть текущим моментом, больше его никак изменять нельзя. Если случилось так, что последний учитываемый день уже закончился, временной период закрывается на момент окончания дня. 
void data_provider :: check_allow_update_time_period (time_period & element) throw (update_storage_error) {
	time_period selected = data_cache<time_period>::get_by_id(element.id);
	if (!selected.is_valid()) throw update_storage_error(L"Ошибка при обновлении временного периода - период с указанным id не найден.");
	if (selected.is_closed()) throw update_storage_error(L"Ошибка при обновлении временного периода - период уже закрыт.");
	day last_day = data_cache<day>::get_last();
	if (!last_day.is_valid()) throw update_storage_error(L"Ошибка при обновлении временного периода - нет учитываемых дней.");
	datetime now;
	element = selected;
	element.end = last_day.end <= now ? last_day.end : now;
	if (!element.is_correct()) throw update_storage_error(L"Ошибка обновления временного периода - некорректные данные для обновления, либо временной период был открыт после окончания последнего учитываемого дня");
}

// Отметки выполнения однократных дел нельзя менять. 
void data_provider :: check_allow_update_work_checked (work_checked & element) throw (update_storage_error) {
	throw update_storage_error(L"Отметки выполнения однократных дел нельзя менять.");
}

const char * dp_sqlite :: filename = "timer.db";
bool dp_sqlite :: db_was_checked = false;
dp_sqlite * dp_sqlite :: obj = 0;

dp_sqlite * dp_sqlite :: get_obj () {
	if (!obj) obj = new dp_sqlite;
	return obj;
};

void dp_sqlite :: free_obj () {
	if (obj) {
		delete obj;
		obj = 0;
	};
};

int dp_sqlite :: callback (void *rows, int argc, char **argv, char **szColName) {
	map <string, string> row;
	for (int i = 0; i < argc; i++) row[string(szColName[i])] = string(argv[i]);
	static_cast<Data_Rows *>(rows)->push_back(row);
	return 0;
};

dp_sqlite :: dp_sqlite () try : db(0), ErrMsg(0) {
	if (sqlite3_open(filename, &db) != SQLITE_OK) throw sqlite_open_error(sqlite3_errmsg(db), filename);
	if (!db_was_checked) {
		// Создаем таблицы при необходимости. 
		const char * init_query[6];
		init_query[0] = "CREATE TABLE IF NOT EXISTS works (id INTEGER PRIMARY KEY, name TEXT, type TEXT, created_datetime TEXT, deleted INTEGER)";
		init_query[1] = "CREATE TABLE IF NOT EXISTS one_works (id INTEGER PRIMARY KEY, work_id INTEGER, count INTEGER)";
		init_query[2] = "CREATE TABLE IF NOT EXISTS long_works (id INTEGER PRIMARY KEY, work_id INTEGER, plan INTEGER)";
		init_query[3] = "CREATE TABLE IF NOT EXISTS days (id INTEGER PRIMARY KEY, datetime_start TEXT, datetime_end TEXT)";
		init_query[4] = "CREATE TABLE IF NOT EXISTS time_periods (id INTEGER PRIMARY KEY, work_id INTEGER, datetime_start TEXT, datetime_end TEXT)";
		init_query[5] = "CREATE TABLE IF NOT EXISTS work_checkeds (id INTEGER PRIMARY KEY, work_id INTEGER, datetime TEXT)";
		for (int i = 0; i < 6; i++) sql_query_exec(init_query[i]);
		db_was_checked = true;
	}
	load_works();
	load_days();
	load_time_periods();
	load_work_checkeds();

} catch (sqlite_open_error & exp) {
	sqlite3_close(db);
	throw;

} catch (sqlite_query_error & exp) {
	sqlite3_close(db);
	string new_message(string("Текст запроса SQL: ") + exp.query_text + "\n" + exp.system_message);
	throw sqlite_open_error(new_message.c_str(), 0, exp.my_message);

} catch (out_of_range & exp) {
	sqlite3_close(db);
	throw sqlite_open_error(0, 0, L"Ошибка БД - не найдены некоторые поля в таблицах. Данные испорчены");
}

dp_sqlite :: ~dp_sqlite () {
	sqlite3_close(db);
};

// Здесь по сути объект удаляет сам себя, но при правильном применении все будет нормально. 
void dp_sqlite :: free () {
	free_obj();
};

string dp_sqlite :: get_ids_string_for_sql (const Data_Rows & rows) try {
	string result = "";
	Data_Rows_Size rows_cont = rows.size();
	if (rows_cont == 1) {
		result = string("= ") + rows[0].at("id");
	} else if (rows_cont > 1) {
		result = "in (";
		for (Data_Rows_Size i = 0; i < rows_cont; ++i) result += rows[i].at("id") + (i == rows_cont - 1 ? "" : ", ");
		result += ")";
	}
	return result;

} catch (out_of_range & exp) {
	throw sqlite_query_error(0, L"Ошибка БД - не найден обязательный столбец id. Данные испорчены");
}

void dp_sqlite :: sql_query_exec (const char * sql, Data_Rows * const rows) throw (sqlite_query_error) {
	int exec_result = rows ? sqlite3_exec(db, sql, callback, rows, &ErrMsg) : sqlite3_exec(db, sql, 0, 0, &ErrMsg);
	if (exec_result != SQLITE_OK) throw sqlite_query_error(ErrMsg, 0, sql);
}

// Корректность данных для atoi не проверяем, так как в БД будет писать только сама программа. 
void dp_sqlite :: load_works () try {
	bool need_one = !data_cache<one_work>::list_loaded;
	bool need_long = !data_cache<long_work>::list_loaded;
	if (need_one or need_long) {
		Data_Rows main_rows;
		string sql = string("SELECT * FROM works") + (need_one and need_long ? "" : (need_one ? " WHERE type = \"one\"" : " WHERE type = \"long\"")) + " ORDER BY id ASC";
		sql_query_exec (sql.c_str(), & main_rows);
		Data_Rows_Size rows_count = main_rows.size();
		if (rows_count > 0) {
			Data_Rows_Size i, one_count = 0, long_count = 0;
			for (i = 0; i < rows_count; ++i) main_rows[i].at("type") == "one" ? ++one_count : ++long_count;
			if (need_one and one_count > 0) {
				sql = "SELECT * FROM one_works";
				Data_Rows one_rows;
				sql_query_exec (sql.c_str(), & one_rows);
				datetime created;
				Data_Rows_Size j, one_rows_count = one_rows.size();
				for (i = 0; i < rows_count; ++i)
					if (main_rows[i].at("type") == "one") {
						bool was_find = false;
						for (j = 0; j < one_rows_count; ++j)
							if (one_rows[j].at("work_id") == main_rows[i].at("id")) {
								if (!created.parse_sqlite_format(main_rows[i].at("created_datetime"))) throw sqlite_query_error(0, L"Ошибка БД - найден неверный формат записи даты-времени. Данные испорчены");
								one_work element(atoi(main_rows[i].at("id").c_str()), str2wstr(main_rows[i].at("name")), atoi(one_rows[j].at("count").c_str()), created, main_rows[i].at("deleted") != "0");
								data_cache<one_work>::list.push_back(element);
								was_find = true;
								one_rows.erase(one_rows.begin() + j);
								--one_rows_count;
								break;
							}
						if (!was_find) throw sqlite_query_error(0, L"Ошибка БД - не найдены данные для однократных дел. Данные испорчены");
					}
			}
			if (need_long and long_count > 0) {
				sql = "SELECT * FROM long_works";
				Data_Rows long_rows;
				sql_query_exec (sql.c_str(), & long_rows);
				datetime created;
				Data_Rows_Size j, long_rows_count = long_rows.size();
				for (i = 0; i < rows_count; ++i)
					if (main_rows[i].at("type") == "long") {
						bool was_find = false;
						for (j = 0; j < long_rows_count; ++j)
							if (long_rows[j].at("work_id") == main_rows[i].at("id")) {
								if (!created.parse_sqlite_format(main_rows[i].at("created_datetime"))) throw sqlite_query_error(0, L"Ошибка БД - найден неверный формат записи даты-времени. Данные испорчены");
								long_work element(atoi(main_rows[i].at("id").c_str()), str2wstr(main_rows[i].at("name")), atoi(long_rows[j].at("plan").c_str()), created, main_rows[i].at("deleted") != "0");
								data_cache<long_work>::list.push_back(element);
								was_find = true;
								long_rows.erase(long_rows.begin() + j);
								--long_rows_count;
								break;
							}
						if (!was_find) throw sqlite_query_error(0, L"Ошибка БД - не найдены данные для продолжительных дел. Данные испорчены");
					}
			}
		}
		data_cache<one_work>::sort();
		data_cache<long_work>::sort();
		data_cache<one_work>::list_loaded = true;
		data_cache<long_work>::list_loaded = true;
	}
	
} catch (out_of_range & exp) {
	throw sqlite_query_error(0, L"Ошибка БД - не найдены некоторые поля в таблицах дел. Данные испорчены");
}

void dp_sqlite :: load_days () try {
	if (!data_cache<day>::list_loaded) {
		const char * sql = "SELECT * FROM days ORDER BY id ASC";
		Data_Rows rows;
		sql_query_exec (sql, & rows);
		Data_Rows_Size i, count = rows.size();
		datetime start, end;
		for (i = 0; i < count; ++i) {
			if (!start.parse_sqlite_format(rows[i].at("datetime_start")) or !end.parse_sqlite_format(rows[i].at("datetime_end")))
				throw sqlite_query_error(0, L"Ошибка БД - найден неверный формат даты-времени в таблице дней. Данные испорчены");
			data_cache<day>::list.push_back(day(atoi(rows[i].at("id").c_str()), start, end));
		}
		data_cache<day>::sort();
		data_cache<day>::list_loaded = true;
	}

} catch (out_of_range & exp) {
	throw sqlite_query_error(0, L"Ошибка БД - не найдены некоторые поля в таблице дней. Данные испорчены");
}

void dp_sqlite :: load_time_periods () try {
	if (!data_cache<time_period>::list_loaded) {
		const char * sql = "SELECT * FROM time_periods ORDER BY id ASC";
		Data_Rows rows;
		sql_query_exec (sql, & rows);
		Data_Rows_Size i, count = rows.size();
		datetime start, end;
		for (i = 0; i < count; ++i) {
			if (
				!start.parse_sqlite_format(rows[i].at("datetime_start"))
				or (rows[i].at("datetime_end") != "" and !end.parse_sqlite_format(rows[i].at("datetime_end")))
			)
				throw sqlite_query_error(0, L"Ошибка БД - найден неверный формат даты-времени в таблице временных периодов. Данные испорчены");
			if (rows[i].at("datetime_end") == "") end.set(0);
			data_cache<time_period>::list.push_back(time_period(atoi(rows[i].at("id").c_str()), atoi(rows[i].at("work_id").c_str()), start, end));
		}
		data_cache<time_period>::sort();
		data_cache<time_period>::list_loaded = true;
	}

} catch (out_of_range & exp) {
	throw sqlite_query_error(0, L"Ошибка БД - не найдены некоторые поля в таблице временных периодов. Данные испорчены");
}

void dp_sqlite :: load_work_checkeds () try {
	if (!data_cache<work_checked>::list_loaded) {
		const char * sql = "SELECT * FROM work_checkeds ORDER BY id ASC";
		Data_Rows rows;
		sql_query_exec (sql, & rows);
		Data_Rows_Size i, count = rows.size();
		datetime check;
		for (i = 0; i < count; ++i) {
			if (!check.parse_sqlite_format(rows[i].at("datetime"))) throw sqlite_query_error(0, L"Ошибка БД - найден неверный формат даты-времени в таблице проверок однократных дел. Данные испорчены");
			data_cache<work_checked>::list.push_back(work_checked(atoi(rows[i].at("id").c_str()), atoi(rows[i].at("work_id").c_str()), check));
		}
		data_cache<work_checked>::sort();
		data_cache<work_checked>::list_loaded = true;
	}

} catch (out_of_range & exp) {
	throw sqlite_query_error(0, L"Ошибка БД - не найдены некоторые поля в таблице проверок однократных дел. Данные испорчены");
}

int dp_sqlite :: add_one_work (one_work val) {
	check_allow_add_one_work(val);
	string sql = "BEGIN; ";
	sql += "INSERT INTO works (id, name, type, created_datetime, deleted) VALUES (" + int_to_string(val.id) + ", \"" + wstr2str(escape_double_quotes(val.name)) + "\", \"one\", \"" + val.created.get_sqlite_format() + "\", " + (val.deleted ? "1" : "0") + "); ";
	sql += "INSERT INTO one_works (work_id, count) VALUES (" + int_to_string(val.id) + ", " + int_to_string(val.count) + "); ";
	sql += "COMMIT;";
	sql_query_exec(sql.c_str());
	data_cache<one_work>::add_element(val);
	return val.id;
}

int dp_sqlite :: add_long_work (long_work val) {
	check_allow_add_long_work(val);
	string sql = "BEGIN; ";
	sql += "INSERT INTO works (id, name, type, created_datetime, deleted) VALUES (" + int_to_string(val.id) + ", \"" + wstr2str(escape_double_quotes(val.name)) + "\", \"long\", \"" + val.created.get_sqlite_format() + "\", " + (val.deleted ? "1" : "0") + "); ";
	sql += "INSERT INTO long_works (work_id, plan) VALUES (" + int_to_string(val.id) + ", " + int_to_string(val.plan) + "); ";
	sql += "COMMIT;";
	sql_query_exec(sql.c_str());
	data_cache<long_work>::add_element(val);
	return val.id;
}

int dp_sqlite :: add_day (day val) {
	check_allow_add_day(val);
	string sql = "INSERT INTO days (id, datetime_start, datetime_end) VALUES (" + int_to_string(val.id) + ", \"" + val.start.get_sqlite_format() + "\", \"" + val.end.get_sqlite_format() + "\")";
	sql_query_exec(sql.c_str());
	data_cache<day>::add_element(val);
	return val.id;
}

int dp_sqlite :: add_time_period (time_period val) {
	check_allow_add_time_period(val);
	string sql = "INSERT INTO time_periods (id, work_id, datetime_start, datetime_end) VALUES (" + int_to_string(val.id) + ", " + int_to_string(val.work_id) + ", \"" + val.start.get_sqlite_format() + "\", \"" + val.end.get_sqlite_format() + "\")";
	sql_query_exec(sql.c_str());
	data_cache<time_period>::add_element(val);
	return val.id;
}

int dp_sqlite :: add_work_checked (work_checked val) {
	check_allow_add_work_checked(val);
	string sql = "INSERT INTO work_checkeds (id, work_id, datetime) VALUES (" + int_to_string(val.id) + ", " + int_to_string(val.work_id) + ", \"" + val.check.get_sqlite_format() + "\")";
	sql_query_exec(sql.c_str());
	data_cache<work_checked>::add_element(val);
	return val.id;
}

void dp_sqlite :: update_one_work (one_work val) {
	check_allow_update_one_work(val);
	string sql = "BEGIN; ";
	sql += "UPDATE works SET name = \"" + wstr2str(escape_double_quotes(val.name)) + "\", created_datetime = \"" + val.created.get_sqlite_format() + "\", deleted = " + (val.deleted ? "1" : "0") + " WHERE id = " + int_to_string(val.id) + "; ";
	sql += "UPDATE one_works SET count = " + int_to_string(val.count) + " WHERE work_id = " + int_to_string(val.id) + "; ";
	sql += "COMMIT;";
	sql_query_exec(sql.c_str());
	data_cache<one_work>::update_element(val);
}

void dp_sqlite :: update_long_work (long_work val) {
	check_allow_update_long_work(val);
	string sql = "BEGIN; ";
	sql += "UPDATE works SET name = \"" + wstr2str(escape_double_quotes(val.name)) + "\", created_datetime = \"" + val.created.get_sqlite_format() + "\", deleted = " + (val.deleted ? "1" : "0") + " WHERE id = " + int_to_string(val.id) + "; ";
	sql += "UPDATE long_works SET plan = " + int_to_string(val.plan) + " WHERE work_id = " + int_to_string(val.id) + "; ";
	sql += "COMMIT;";
	sql_query_exec(sql.c_str());
	data_cache<long_work>::update_element(val);
}

void dp_sqlite :: update_day (day val) {
	check_allow_update_day(val);
	string sql = "UPDATE days SET datetime_start = \"" + val.start.get_sqlite_format() + "\", datetime_end = \"" + val.end.get_sqlite_format() + "\" WHERE id = " + int_to_string(val.id);
	sql_query_exec(sql.c_str());
	data_cache<day>::update_element(val);
}

void dp_sqlite :: update_time_period (time_period val) {
	check_allow_update_time_period(val);
	string sql = "UPDATE time_periods SET work_id = " + int_to_string(val.work_id) + ", datetime_start = \"" + val.start.get_sqlite_format() + "\", datetime_end = \"" + val.end.get_sqlite_format() + "\" WHERE id = " + int_to_string(val.id);
	sql_query_exec(sql.c_str());
	data_cache<time_period>::update_element(val);
}

void dp_sqlite :: update_work_checked (work_checked val) {
	check_allow_update_work_checked(val);
	string sql = "UPDATE work_checkeds SET work_id = " + int_to_string(val.work_id) + ", datetime = \"" + val.check.get_sqlite_format() + "\" WHERE id = " + int_to_string(val.id);
	sql_query_exec(sql.c_str());
	data_cache<work_checked>::update_element(val);
}

void dp_sqlite :: delete_one_work (int id) {
	data_cache<one_work>::iterator element = data_cache<one_work>::get_iterator_by_id(id);
	if (element != data_cache<one_work>::end()) {
		string sql = "BEGIN; ";
		sql += "DELETE FROM works WHERE id = " + int_to_string(id) + "; ";
		sql += "DELETE FROM one_works WHERE work_id = " + int_to_string(id) + "; ";
		sql += "DELETE FROM work_checkeds WHERE work_id = " + int_to_string(id) + "; ";
		sql += "COMMIT;";
		sql_query_exec(sql.c_str());
		data_cache<one_work>::erase(element);
		data_cache<work_checked>::delete_by_class_member(& work_checked::work_id, id);
	}
}

void dp_sqlite :: delete_long_work (int id) {
	data_cache<long_work>::iterator element = data_cache<long_work>::get_iterator_by_id(id);
	if (element != data_cache<long_work>::end()) {
		string sql = "BEGIN; ";
		sql += "DELETE FROM works WHERE id = " + int_to_string(id) + "; ";
		sql += "DELETE FROM long_works WHERE work_id = " + int_to_string(id) + "; ";
		sql += "DELETE FROM time_periods WHERE work_id = " + int_to_string(id) + "; ";
		sql += "COMMIT;";
		sql_query_exec(sql.c_str());
		data_cache<long_work>::erase(element);
		data_cache<time_period>::delete_by_class_member(& time_period::work_id, id);
	}
}

data_provider * dp (char res) {
	return dp_sqlite::get_obj();
};
