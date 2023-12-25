#include <logger/logger.h>

namespace logger {

Logger* Logger::instance = nullptr;
Output Logger::output = Output::CONSOLE;
Level Logger::minimumLogLevel = Level::INFO;
std::filesystem::path Logger::logFilePath = std::filesystem::path();

std::string LevelArray[]{"DEBUG", "INFO", "WARNING", "ERROR"};

Logger::Logger(Level minLogLevel) {
  Logger::output = Output::CONSOLE;
  Logger::minimumLogLevel = minLogLevel;
}

Logger::Logger(Output out, std::filesystem::path filePath, Level minLogLevel) {
  Logger::output = out;
  Logger::logFilePath = filePath;
  Logger::minimumLogLevel = minLogLevel;
}

}  // namespace logger