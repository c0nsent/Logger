/**
 * @date 30-05-2025
 * @author consent_
 */


#include "Logger.hpp"

#include <ctime>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <cmath>
#include <iomanip>


namespace lrh
{
	std::ostream &operator<<( std::ostream &os, const Logger::Level lvl )
	{
		os << Logger::CHAR_LVL_ARRAY[static_cast<int>(lvl)];

		return os;
	}


	void Logger::info( const std::string &message, const sl &loc )
	{
		init().write( message, Level::Info, loc );
	}


	void Logger::debug( const std::string &message, const sl &loc )
	{
		init().write( message, Level::Debug, loc );
	}


	void Logger::warning( const std::string &message, const sl &loc )
	{
		init().write( message, Level::Warning, loc );
	}


	void Logger::error( const std::string &message, const sl &loc )
	{
		init().write( message, Level::Error, loc );
	}


	void Logger::fatal( const std::string &message, const sl &loc )
	{
		init().write( message, Level::Fatal, loc );
	}




	Logger::Logger( const std::string &fileName )
		: m_loggerStream( fileName, std::ios::app )
	{
		if( not m_loggerStream.is_open() )
			throw std::runtime_error( "Could not open log file: " + fileName );
	}


	Logger &Logger::instance()
	{
		static Logger instance{ createFileName( .string() ) };
		return instance;
	}


	void Logger::write( const std::string &message, const Level lvl, const sl &loc )
	{
		constexpr static auto format{ "%Y %m %d | %H:%M:%S" };

		std::stringstream log;

		log << lvl << "\t: " << '[' << getCurrentDateTime( format ) << " | " << getFileName( loc.file_name() )
				<< "\t| " << loc.function_name() << " | " << loc.line() << "] : " << message << '\n';

		m_loggerStream << log.str();

		std::cout << log.str() << std::endl;

		m_loggerStream.flush();
	}


	std::string Logger::createFileName( const std::string &logsLocation )
	{
		std::stringstream fileName;

		if ( std::filesystem::create_directory( logsLocation ) )
		{

			fileName << logsLocation;

			if ( logsLocation.back() != '/' )
				fileName << '/';
		}

		fileName << getCurrentDateTime( "%Y_%m_%d_" )
			<< logIdToString( getLogID( logsLocation.data() ) ) << ".log";

		return fileName.str();
	}


	const char *Logger::getCurrentDateTime( const char *format )
	{
		const std::time_t currentTime{ std::time( nullptr ) };
		const std::tm *structTime{ std::localtime( &currentTime ) };

		constexpr static std::size_t bufferSize{ 25 };
		static char buffer[bufferSize];

		std::strftime( buffer, bufferSize, format, structTime );

		return buffer;
	}


	int Logger::getLogID( const char *logsLocation )
	{
		namespace fs = std::filesystem;

		constexpr static auto logNameFormat{ "%Y_%m_%d_" };
		const static fs::path path( logsLocation );
		const static auto currentDate{ getCurrentDateTime( logNameFormat ) };

		int id{ 1 };
		for( const fs::directory_entry &entry : fs::directory_iterator( logsLocation ) )
		{
			const std::string fileName{ entry.path().filename() };

			///Здесь строка а не char для использования методов строки
			if( fileName.starts_with( currentDate ) ) id++;
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
