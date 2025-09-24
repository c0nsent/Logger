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

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <source_location> ///< Библа для вывода названия функции и файла
#include <string>
#include <string_view>

namespace lrh
{
	class Logger
	{
		using sl =  std::source_location;

		static constexpr const char *CHAR_LVL_ARRAY[]
		{
			"INFO", "DEBUG", "WARNING", "ERROR", "FATAL"
		};

		static constexpr std::string_view DEFAULT_LOG_DIR{"logs/"};
		static constexpr std::string_view TIME_FORMAT{ "%H:%M:%S" };
		static constexpr std::string_view DATE_FORMAT{ "%Y_%m_%d" };

		///enum для простоты определения уровня лога
		enum class Level : uint8_t
		{
			Info, Debug, Warning, Error, Fatal
		};

		friend std::ostream& operator<<(std::ostream &os, Level lvl);


	public:

		///Инициализация синглтона
		static Logger &instance();

		///@brief Враппер над write() для удобства работы с логгером
		///@param loc По-умолчанию передает функция, где вызван логгер
		static void info(const std::string &message, const sl &loc = sl::current());
		static void debug(const std::string &message, const sl &loc = sl::current());
		static void warning(const std::string &message, const sl &loc = sl::current());
		static void error(const std::string &message, const sl &loc = sl::current());
		static void fatal(const std::string &message, const sl &loc = sl::current());


		///Копирование и присваивание не разрешены,
		///так как это синглтон
		Logger(const Logger &) = delete;
		Logger(Logger &&) = delete;
		Logger& operator=(const Logger &) = delete;
		Logger& operator=(Logger &&) = delete;

	private:
		explicit Logger(std::string_view fileName);

		///Выводит сообщение в лог
		void write( std::string_view message, Level lvl, const sl& loc );

		static bool tryCreateDirectory(std::string_view logsDir);
		///@brief Возвращает строку с именем файла в формате "2025_05_30_001.log"
		static std::string createLogFile( std::string_view logsDir );
		static std::string getCurrentDateTime(std::string_view format);

		///@brief Возвращает id для лог-файла
		static int getLogID( const char* logsPath );
		///@brief Возвращает строковый id в формате "001"
		static std::string logIdToString(uint16_t id);
		///@brief Возвращает название файла, убирая путь
		static const char* getFileName(const std::string &fullPath);

		std::ofstream m_loggerStream; ///< Поток вывода в файл
	};
}