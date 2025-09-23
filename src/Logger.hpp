/**
 * @date 30-05-2025
 * @author consent_
 *
 * @brief Простенький логгер для игры.
 *
 * @details При вызове функций логгера создает файл в директории "logs/"
 * с названием в формате "2025_05_30_001.log" и записывает в него сообщения
 * в формате:
 *
 * INFO	: [2025 05 30 | 16:23:53 | main.cpp	| int main() | 49] : Shooting stars never stop
 */

#pragma once

#include <fstream>
#include <source_location> ///< Библа для вывода названия функции и файла
#include <string>
//#include <string_view>

namespace lrh
{
	class Logger
	{
		using sl =  std::source_location;

		///enum для простоты определения уровня лога
		enum class Level : uint8_t
		{
			Info, Debug, Warning, Error, Fatal
		};

		friend std::ostream& operator<<(std::ostream &ss, Level lvl);

	public:

		explicit Logger(const std::string &fileName);

		std::string getLogDir() const;

		///@brief Враппер над write() для удобства работы с логгером
		///@param loc По-умолчанию передает функция, где вызван логгер
		void info(const std::string &message, const sl &loc = sl::current());
		void debug(const std::string &message, const sl &loc = sl::current());
		void warning(const std::string &message, const sl &loc = sl::current());
		void error(const std::string &message, const sl &loc = sl::current());
		void fatal(const std::string &message, const sl &loc = sl::current());

	private:

		///Выводит сообщение в лог
		void write( const std::string &message, Level lvl, const sl& loc );

		///@brief Возвращает строку с именем файла в формате "2025_05_30_001.log"
		static std::string createFileName(const std::string &logsLocation );
		static const char* getCurrentDateTime(const char *format);

		///@brief Возвращает id для лог-файла
		static int getLogID(const char* logsLocation);
		///@brief Возвращает строковый id в формате "001"
		static std::string logIDtoStr(int id);
		///@brief Возвращает название файла, убирая путь
		static const char* getFileName(const std::string &fullPath);

		static constexpr const char *CHAR_LVL_ARRAY[]
		{
			"INFO", "DEBUG", "WARNING", "ERROR", "FATAL"
		};

		static constexpr const char *DEFAULT_LOG_DIR{"logs/"};

		std::string m_logDir;
		std::ofstream m_loggerStream; ///< Поток вывода в файл
	};
}