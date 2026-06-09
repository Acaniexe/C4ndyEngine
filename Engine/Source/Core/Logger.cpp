#include "Logger.h"
#include <iostream>

#define C4NDY_COLOR_RESET    "\033[0m"
#define C4NDY_COLOR_WHITE    "\033[37m"
#define C4NDY_COLOR_YELLOW   "\033[33m"
#define C4NDY_COLOR_RED      "\033[31m"
#define C4NDY_COLOR_MAGENTA  "\033[35m"

namespace C4ndy
{
    Logger& Logger::Get()
    {
        static Logger instance;
        return instance;
    }

    void Logger::Log(LogLevel level, const std::string& message)
    {
        const char* colour = C4NDY_COLOR_WHITE;
        const char* prefix = "[INFO]  ";
        switch (level)
        {
            case LogLevel::Info:    colour = C4NDY_COLOR_WHITE;   prefix = "[INFO]  "; break;
            case LogLevel::Warn: colour = C4NDY_COLOR_YELLOW;  prefix = "[WARN]  "; break;
            case LogLevel::Error:   colour = C4NDY_COLOR_RED;     prefix = "[ERROR]  "; break;
            case LogLevel::Fatal:   colour = C4NDY_COLOR_MAGENTA; prefix = "[FATAL]  "; break;
        }

        std::cout << colour << prefix << message << C4NDY_COLOR_RESET << "\n";
        
        if (_fileLoggingEnabled && _logFile.is_open())
            _logFile << prefix << message << "\n";
    }

    void Logger::Info(const std::string& message)
    {
        Log(LogLevel::Info, message);
    }

    void Logger::Warn(const std::string& message)
    {
        Log(LogLevel::Warn, message);
    }

    void Logger::Error(const std::string& message)
    {
        Log(LogLevel::Error, message);
    }

    void Logger::Fatal(const std::string& message)
    {
        Log(LogLevel::Fatal, message);
    }

    void Logger::EnableFileLogging(const std::string& filePath)
    {
        _logFilePath = filePath;
        _logFile.open(filePath, std::ios::out | std::ios::app);
        _fileLoggingEnabled = _logFile.is_open();

        if (!_fileLoggingEnabled)
            Log(LogLevel::Warn, "Could not open log file: " + filePath);
    }
}