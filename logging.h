#ifndef LOGGING_H
#define LOGGING_H

#include <QDir>
#include <QMessageBox>
#include <QCoreApplication>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>

class Logging
{
public:
  static void logInfo(const std::string& message);
  static void logError(const std::string& message);
  static void logWarning(const std::string& message);

private:
 static std::shared_ptr<spdlog::logger>getLogger();
 static std::shared_ptr<spdlog::logger>logger;
};

#endif // LOGGING_H
