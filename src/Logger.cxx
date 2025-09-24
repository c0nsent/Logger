/**
 * @date 30-05-2025
 * @author consent_
 */


#include "Logger.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>


namespace lrh
{
	std::ostream &operator<<( std::ostream &os, const Logger::Level lvl )
	{
		os << Logger::CHAR_LVL_ARRAY[static_cast<int>(lvl)];

		return os;
	}


	void Logger::info( const std::string &message, const sl &loc )
	{
		instance().write( message, Level::Info, loc );
	}


	void Logger::debug( const std::string &message, const sl &loc )
	{
		instance().write( message, Level::Debug, loc );
	}


	void Logger::warning( const std::string &message, const sl &loc )
	{
		instance().write( message, Level::Warning, loc );
	}


	void Logger::error( const std::string &message, const sl &loc )
	{
		instance().write( message, Level::Error, loc );
	}


	void Logger::fatal( const std::string &message, const sl &loc )
	{
		instance().write( message, Level::Fatal, loc );
	}


	Logger::Logger( const std::string_view fileName )
		: m_loggerStream( fileName.data(), std::ios::app )
	{
		if( not m_loggerStream.is_open() )
			throw std::ios_base::failure( "Could not open new log file" );
	}


	Logger &Logger::instance()
	{
		static Logger instance{ createLogFile( DEFAULT_LOG_DIR.data() ) };
		return instance;
	}


	void Logger::write( const std::string_view message, const Level lvl, const sl &loc )
	{
		std::stringstream log;

		log << lvl << "\t: " << '[' << getCurrentDateTime( TIME_FORMAT.data() ) << " | " << loc.file_name()
				<< "\t| " << loc.function_name() << " | " << loc.line() << "] : " << message << '\n';

		m_loggerStream << log.str();

		std::cout << log.str() << std::endl;

		m_loggerStream.flush();
	}


	bool Logger::tryCreateDirectory( const std::string_view logsDir )
	{
		if ( std::filesystem::exists( logsDir ))
			return std::filesystem::is_directory( logsDir );

		return std::filesystem::create_directory( logsDir );
	}


	std::string Logger::createLogFile( const std::string_view logsDir )
	{
		if (not tryCreateDirectory( logsDir ))
			throw std::filesystem::filesystem_error( "Could not create logs directory", std::error_code());

		std::stringstream fileName;

		fileName << logsDir << getCurrentDateTime( DATE_FORMAT.data() )
			<< logIdToString( getLogID( logsDir.data() ) ) << ".log";

		return fileName.str();
	}


	std::string Logger::getCurrentDateTime( const std::string_view format )
	{
		const auto now{ std::chrono::system_clock::now() };
		const auto inTimeT{ std::chrono::system_clock::to_time_t( now ) };

		std::ostringstream oss;
		oss << std::put_time(std::localtime( &inTimeT ), format.data());

		return oss.str();
	}


	int Logger::getLogID( const char *logsPath )
	{
		const std::filesystem::path path{ logsPath };
		const auto currentDate{ getCurrentDateTime( "%Y_%m_%d_" ) };

		int id{ 1 };
		for( const std::filesystem::directory_entry &entry : std::filesystem::directory_iterator( logsPath ) )
		{
			entry.is_regular_file()
			///Здесь строка а не char для использования методов строки
			//if( fileName.starts_with( currentDate ) ) id++;
		}
		return id;
	}


	std::string Logger::logIdToString( const uint16_t id )
	{
		if ( id > 10000 ) throw std::runtime_error( "Log ID is too big. Too many logs" );

		std::ostringstream oss;
		oss << std::setfill( '0' ) << std::setw( 4 ) << id;

		return oss.str();
	}

	/**
	* \details Возвращает указатель на fullPath, который указывает
	* на начало названия самого файла
	*/
	const char *Logger::getFileName( const std::string &fullPath )
	{
		const std::size_t pos = fullPath.find_last_of( '/' );
		return &fullPath[pos + 1];
	}
}
