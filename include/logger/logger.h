#ifndef CNF_CONVERTOR_LOGGER_LOGGER
#define CNF_CONVERTOR_LOGGER_LOGGER

#include <error/error.h>
#include <error/unknown.h>
#include <warning/warning.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <variant>

namespace logger {

enum Level { DEBUG, INFO, WARNING, ERROR };

extern std::string LevelArray[];

enum Output { FILE, CONSOLE, BOTH };

struct debugLog {
  std::string log;
};

struct infoLog {
  std::string log;
};

struct warnLog {
  warning::warning warning;
};

struct errorLog {
  error::error error;
};

class Logger {
 private:
  static std::unique_ptr<Logger> instance;
  static Output output;
  static Level minimumLogLevel;
  static std::filesystem::path logFilePath;

  static void writeLog(std::string log) {
    if (output == Output::CONSOLE || output == Output::BOTH) {
      std::cout << log << std::endl;
    }
    if (output == Output::FILE || output == Output::BOTH) {
      std::ofstream logFile;
      logFile.open(logFilePath, std::ios::app);
      if (logFile.failbit) {
        throw std::runtime_error("FATAL: File " + logFilePath.string() +
                                 " cannot be opened");
      }
      logFile << log << std::endl;
      logFile.close();
    }
  }

 public:
  Logger(Level minimumLogLevel);
  Logger(Output output, std::filesystem::path logFilePath,
         Level minimumLogLevel);
  Logger(const Logger& obj) = delete;
  static void initLogger(Level minimumLogLevel) {
    if (instance == nullptr) {
      instance = std::make_unique<Logger>(minimumLogLevel);
    }
  }

  static void initLogger(Output output, std::filesystem::path logFile,
                         Level minimumLogLevel) {
    if (instance == nullptr) {
      instance = std::make_unique<Logger>(output, logFile, minimumLogLevel);
    }
  }

  static std::variant<Level, error::unknown::unknown_error> getLevel(
      std::variant<debugLog, infoLog, warnLog, errorLog> log) {
    if (std::holds_alternative<debugLog>(log)) {
      return Level::DEBUG;
    } else if (std::holds_alternative<infoLog>(log)) {
      return Level::INFO;
    } else if (std::holds_alternative<warnLog>(log)) {
      return Level::WARNING;
    } else if (std::holds_alternative<errorLog>(log)) {
      return Level::ERROR;
    } else {
      return error::unknown::unknown_error(
          "Unknown error alternative passed to getLevel()");
    }
  }

  static void dispatchLog(
      std::variant<debugLog, infoLog, warnLog, errorLog> log) {
    if (instance == nullptr) return;
    const auto levelType = getLevel(log);

    if (std::holds_alternative<error::unknown::unknown_error>(levelType)) {
      Logger::dispatchLog(
          errorLog{error : std::get<error::unknown::unknown_error>(levelType)});
    } else if (std::holds_alternative<Level>(levelType)) {
      const auto level = std::get<Level>(levelType);
      if (level >= minimumLogLevel) {
        const auto now = std::chrono::system_clock::now();
        const std::time_t time = std::chrono::system_clock::to_time_t(now);
        std::stringstream logOutput;

        logOutput << "[" << std::put_time(std::localtime(&time), "%FT%TZ")
                  << "]";
        logOutput << "\t" << LevelArray[level] << "\t";

        switch (level) {
          case DEBUG:
            logOutput << std::get<debugLog>(log).log;
            break;
          case INFO:
            logOutput << std::get<infoLog>(log).log;
            break;
          case WARNING:
            logOutput << "Warning \"" << std::get<warnLog>(log).warning.warning
                      << "\" raised due to \""
                      << std::get<warnLog>(log).warning.cause << "\"";
            break;
          case ERROR:
            logOutput << "Error \"" << std::get<errorLog>(log).error.error
                      << "\" raised due to \""
                      << std::get<errorLog>(log).error.cause << "\"";
            break;

          default:
            Logger::dispatchLog(errorLog{
              error : error::unknown::unknown_error(
                  "Unknown error alternative passed to dispatchLog()")
            });
        }

        writeLog(logOutput.str());
      }
    } else {
      Logger::dispatchLog(errorLog{
        error : error::unknown::unknown_error(
            "Unknown level alternative returned by getLevel()")
      });
    }
  }
};

}  // namespace logger

#endif  // CNF_CONVERTOR_LOGGER_LOGGER
