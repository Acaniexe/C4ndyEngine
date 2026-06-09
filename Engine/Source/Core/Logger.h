#pragma once
#include "Core.h"
#include <fstream>

namespace C4ndy
{
    enum class LogLevel
    {
        Info,
        Warn,
        Error,
        Fatal
    };

    class Logger
    {
    public:
        static Logger& Get();
        void Log(LogLevel level, const std::string& message);
        void Info(const std::string& message);
        void Warn(const std::string& message);
        void Error(const std::string& message);
        void Fatal(const std::string& message);
        void EnableFileLogging(const std::string& filePath);

    private:
        Logger() = default;
        C4NDY_NON_COPYABLE_MOVEABLE(Logger)

        std::string _logFilePath;
        std::ofstream _logFile;
        bool _fileLoggingEnabled = false;
    };
}