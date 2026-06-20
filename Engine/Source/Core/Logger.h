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

#define C4NDY_INFO(...)  ::C4ndy::Logger::Get().Info (__VA_ARGS__)
#define C4NDY_WARN(...)  ::C4ndy::Logger::Get().Warn (__VA_ARGS__)
#define C4NDY_ERROR(...) ::C4ndy::Logger::Get().Error(__VA_ARGS__)
#define C4NDY_FATAL(...) ::C4ndy::Logger::Get().Fatal(__VA_ARGS__)