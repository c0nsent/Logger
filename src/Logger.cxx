/**
 * @date 30-05-2025
 * @author consent_
 */

#include "Logger.hpp"

#include <sstream>
#include <ctime>
#include <filesystem>

namespace lrh
{
    std::ostream& operator<<(std::ostream& ss, const Logger::Level level)
    {
        constexpr static const char* arrStrLvl[]
        {
            "INFO", "DEBUG", "WARNING", "ERROR", "FATAL"
        };

        ss << arrStrLvl[static_cast<int>(level)];

        return ss;
    }

    void Logger::info(const std::string& message, const sl& loc)
    {
        getInstance().write(message, Level::Info, loc);
    }

    void Logger::debug(const std::string& message, const sl& loc)
    {
        getInstance().write(message, Level::Debug, loc);
    }

    void Logger::warning(const std::string& message, const sl& loc)
    {
        getInstance().write(message, Level::Warning, loc);
    }

    void Logger::error(const std::string& message, const sl& loc)
    {
        getInstance().write(message, Level::Error, loc);
    }

    void Logger::fatal(const std::string& message, const sl& loc)
    {
        getInstance().write(message, Level::Fatal, loc);
    }

    Logger::Logger(const std::string& fileName)
    {
        ofs.open(fileName, std::ios::app);

        if (not ofs.is_open())
            throw std::runtime_error("Could not open log file: " + fileName);
    }

    Logger& Logger::getInstance()
    {
        static Logger singleton(createFileName());
        return singleton;
    }

    void Logger::write(
        const std::string& message,
        const Level lvl,
        const sl& loc
    )
    {
        constexpr static auto format{"%Y %m %d | %H:%M:%S"};

        std::stringstream log;

        log << lvl << "\t: "
            << '[' << getCurrentDateTime(format) << " | "
            << getFileName(loc.file_name()) << "\t| "
            << loc.function_name() << " | "
            << loc.line() << "] : "
            << message << '\n';

        ofs << log.str();

        ofs.flush();
    }

    std::string Logger::createFileName(const char* logsLocation)
    {
        std::filesystem::create_directory(logsLocation);

        std::stringstream fileName;

        fileName << logsLocation << getCurrentDateTime("%Y_%m_%d_")
            << formatLogID(getLogID(logsLocation)) << ".log";

        return fileName.str();
    }

    const char* Logger::getCurrentDateTime(const char* format)
    {
        const std::time_t currentTime{std::time(nullptr)};
        const std::tm* structTime{std::localtime(&currentTime)};

        constexpr static std::size_t bufferSize{25};
        static char buffer[bufferSize];

        std::strftime(buffer, bufferSize, format, structTime);

        return buffer;
    }

    int Logger::getLogID(const char* logsLocation)
    {
        namespace fs = std::filesystem;

        constexpr static auto logNameFormat{"%Y_%m_%d_"};
        const static fs::path path(logsLocation);
        const static auto currentDate{getCurrentDateTime(logNameFormat)};

        int id{1};
        for (const fs::directory_entry& entry : fs::directory_iterator(logsLocation))
        {
            const std::string fileName{entry.path().filename()};

            ///Здесь строка а не char для использования методов строки
            if (fileName.starts_with(currentDate)) id++;
        }
        return id;
    }

    std::string Logger::formatLogID(const int id)
    {
        std::string strID{std::to_string(id)};
        const auto zeros = static_cast<int>(3 - strID.length());

        strID.insert(strID.begin(), zeros, '0');

        return strID;
    }

    /**
    * \details Возвращает указатель на fullPath, который указывает
    * на начало названия самого файла
    */
    const char* Logger::getFileName(const std::string &fullPath)
    {
        const std::size_t pos = fullPath.find_last_of('/');
        return &fullPath[pos + 1];
    }
}