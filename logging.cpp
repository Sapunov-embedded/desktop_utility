#include "logging.h"


// Инициализация статического члена класса
std::shared_ptr<spdlog::logger> Logging::logger = nullptr;

std::shared_ptr<spdlog::logger> Logging::getLogger() {
    if (!logger) {

        QString logDirPath = QCoreApplication::applicationDirPath() + "/logs";

        QDir dir(logDirPath);
        if (!dir.exists()) {
            if (!dir.mkpath(".")) {
                QMessageBox::critical(nullptr, "Ошибка создания каталога",
                                      "Не удалось создать папку для логов: " + logDirPath);
                throw std::runtime_error("Failed to create log directory");
            }
        }


        logger = spdlog::rotating_logger_mt("file_logger", logDirPath.toStdString() + "/logs.txt", 1 * 1024 * 1024, 3);

        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S] [%l] %v");
    }
    return logger;
}

void Logging::logInfo(const std::string &message) {
    getLogger()->info(message);
}

void Logging::logError(const std::string &message) {
    getLogger()->error(message);
}

void Logging::logWarning(const std::string &message) {
    getLogger()->warn(message);
}
