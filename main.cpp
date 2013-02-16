#include <locale>

#include "exceptions.h"
#include "data_provider.h"
#include "main_module.h"

int main (int argc, char ** argv) {

	std::locale::global(std::locale("")); // Настройка русского ввода-вывода. 
	std::ios::sync_with_stdio(false); // Позволяет одновременно использовать cout и wcout. Но после каждого вывода нужно использовать flush. 

	try {

		dp(); // Обращение к хранилищу данных и загрузка данных в кэш. Теперь можно обращаться напрямую к кэшу, не задумываясь о том, загружены в него данные или еще нет. 
		main_module::start();
		
	} catch (storage_open_error & exp) {

		exp.print_message();
		// Так как ошибка при открытии хранилища, dp()->free() не вызываем. 
		return 1;

	} catch (storage_query_error & exp) {

		exp.print_message();
		dp()->free();
		return 1;

	} catch (...) {

		dp()->free();
		throw;
	}

	dp()->free(); // Обязательное отключение от хранилища для правильной очистки кэша и что надо еще.
	return 0;
}
