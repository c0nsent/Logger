/**
 * @date 30-05-2025
 * @author consent_
 *
 * @brief Синглтон логгер
 *
 * @details
 * При вызове функций логгера создает файл в директории "logs/" с названием в формате "2025_05_30_001.log"
 * и записывает в него сообщения в формате:
 * WARNING:  Shooting stars never stop	[ 15 | int main() | /home/amitayus_/Programming/C++/Logger/test/test.cxx	| 19:57:39 ]
 */


#pragma once


#include <cstdint>
#include <fstream>
#include <source_location> ///< Библа для вывода названия функции и файла
#include <string>
#include <string_view>


namespace lrh
{
	class Logger
	{
		using sl =  std::source_location;


		/** @brief C-ишный массив для вывода уровня лога в строковом формате*/
		static constexpr const char *CHAR_LVL_ARRAY[]
		{
			"INFO:", "DEBUG:", "WARNING:", "ERROR:", "FATAL:"
		};

		static constexpr std::string_view DEFAULT_LOG_DIR{"logs/"};

		enum class Level : uint8_t
		{
			Info, Debug, Warning, Error, Fatal
		};

		friend std::ostream& operator<<(std::ostream &os, Level lvl);

	public:

		/**
		 * @brief Враппер над write() для удобства работы с логгером
		 *
		 * @param message Сообщение, которое будет записано в лог
		 * @param loc По-умолчанию передает в функцию место вызова функции
		*/
		static void info(const std::string &message, const sl &loc = sl::current());
		static void debug(const std::string &message, const sl &loc = sl::current());
		static void warning(const std::string &message, const sl &loc = sl::current());
		static void error(const std::string &message, const sl &loc = sl::current());
		static void fatal(const std::string &message, const sl &loc = sl::current());


		/** Копирующий конструктор, оператор присваивания и их move вариации удалены, т.к. синглтон */
		Logger(const Logger &) = delete;
		Logger(Logger &&) = delete;
		Logger& operator=(const Logger &) = delete;
		Logger& operator=(Logger &&) = delete;

	private:

		/**
		 * @brief Приватный констуктор, инициализирующий поток
		 *
		 * @details
		 * Создает файл c названием переданном в параметре fileName.
		 * При невозможности открыть поток выбрасывает исключение std::ios_base::failure()
		 */
		explicit Logger(std::string_view fileName);


		/**
		 * @brief Инициализирует сиглтон
		 *
		 * @return Ссылка на экземляр класса
		 *
		 * @details
		 * Вызывает функцию tryCreateDirectory(), выбрасывая std::filesystem::filesystem_error(), если функция вернула false.
		 * Инициализирует статический экземпляр класса, к которому можно
		 * обратиться только по ссылке
		*/
		static Logger &instance();


		/** @brief Метод, записывающий сообщения в лог-файл */
		void write( std::string_view message, Level lvl, const sl &loc );


		/**
		 * @brief Возвращает строку с названием файла
		 *
		 * @details Генерирует название динамически в формате "2025_05_30_001.log"
		 */
		static std::string generateLogFileName( std::string_view logsDir );


		/**
		 * @brief Пытается директорию, возвращая bool с результатом работы.
		 *
		 * @return Возвращает true если директория уже существовала или она была создана.
     *
		 * @details
		 * Проверяет рабочую директорию на наличие файла со соотвествующим названием, если файл директория возвращает true.
		 * При не нахождении файла, пытается создать одноименный файл, возвращаю результат bool с результатом операции.
		 */
		static bool tryCreateLogDirectory(std::string_view logsDir);


		/**
		 * @brief Генерирует 4-х значный id для лога
		 *
		 * @param datePrefix Строка, по которой ищатся подходящие логи
		 *
		 * @return Id лога, который не может быть больше 9'999
		 *
		 * @details
		 * Итерирует logDir, в поиске файлов которые начинаются с datePrefix( "%Y_%m_%d_" )  и имеют расширение ".log".
		 * Сравнивает их id для нахождения maxId. Если maxId равен 9'999 выбрасывает std::runtime_error().
		 * Возвращает maxId + 1.
		 */
		static uint16_t generateLogId( std::string_view logDir, std::string_view datePrefix  );


		/**
		 * @brief Преобразует id в std::string и добавляет отступ, чтобы длина строки была 4 символаю
		 *
		 * @return Строку формата: "0001"
		 */
		static std::string idToPaddedStr( uint16_t id );


		/** @brief Возвращает текущее время и дату в заданном формате */
		static std::string getCurrentDateTime(std::string_view format);


		std::ofstream m_loggerStream; ///< Поток вывода в файл
	};
}