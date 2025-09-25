/**
 * @date 30-05-2025
 * @author consent_
 */


#include "Logger.hpp"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <sstream>


namespace lrh
{
	std::ostream &operator<<( std::ostream &os, const Logger::Level lvl )
	{
		os << Logger::CHAR_LVL_ARRAY[static_cast<uint8_t>(lvl)];

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
		/// Чтобы при каждом вызове метод не проверял наличие папки
		static const bool isDirectoryCreated{ tryCreateLogDirectory( DEFAULT_LOG_DIR.data() ) };
		if ( not isDirectoryCreated )
			throw std::filesystem::filesystem_error( "Could not create logs directory", std::error_code());

		static Logger instance{ generateLogFileName( DEFAULT_LOG_DIR.data() ) };
		return instance;
	}


	void Logger::write( const std::string_view message, const Level lvl, const sl &loc )
	{
		m_loggerStream << std::left << std::setw(10) << lvl << message
			<< "\t[ " << loc.line() << " | "  << loc.function_name() << " | " << loc.file_name()  << "\t| "
			<< getCurrentDateTime( "%H:%M:%S"  ) << " ]" << std::endl;
	}


	bool Logger::tryCreateLogDirectory( const std::string_view logsDir )
	{
		if ( std::filesystem::exists( logsDir ))
			return std::filesystem::is_directory( logsDir );

		return std::filesystem::create_directory( logsDir );
	}


	std::string Logger::generateLogFileName( const std::string_view logsDir )
	{
		const auto datePrefix{ getCurrentDateTime( "%Y_%m_%d_" ) };

		/// Возможно оптимизировать логгер за счет вызова std::ofstream в самих фунциях заместо возврата множества std::string

		std::stringstream fileName;
		fileName << logsDir << datePrefix
			<< idToPaddedStr( generateLogId( logsDir.data(), datePrefix ) ) << ".log";

		return fileName.str();
	}


	std::string Logger::getCurrentDateTime( const std::string_view format )
	{
		using std::chrono::system_clock;
		const auto now{ system_clock::to_time_t( system_clock::now() ) };

		std::ostringstream oss;
		oss << std::put_time(std::localtime( &now ), format.data());

		return oss.str();
	}


	uint16_t Logger::generateLogId( const std::string_view logDir, const std::string_view datePrefix )
	{
		namespace fs = std::filesystem;

		const fs::directory_iterator logDirIt{ logDir, fs::directory_options::skip_permission_denied };

		int maxId{ 0 };

		for ( const auto &entry : logDirIt )
		{
			if ( not entry.is_regular_file() ) continue;

			const auto fileName{ entry.path().filename().string() };

			if (not fileName.starts_with( datePrefix )) continue;
			if (entry.path().extension() != ".log") continue;

			const auto dotPos{ fileName.find( '.', datePrefix.length() ) };
			const auto idStr{ fileName.substr( datePrefix.length(), dotPos - datePrefix.length() ) };

			if ( not std::ranges::all_of(idStr, ::isdigit) ) continue;

			maxId = std::max( maxId, std::stoi( idStr ) );
		}

		if ( maxId  == 9999 )
			throw std::runtime_error( "Log ID is too big. Too many logs" );

		return static_cast<uint16_t>( maxId ) + 1;
	}


	std::string Logger::idToPaddedStr( const uint16_t id )
	{
		std::ostringstream oss;
		oss << std::setfill( '0' ) << std::setw( 4 ) << id;

		return oss.str();
	}
}
