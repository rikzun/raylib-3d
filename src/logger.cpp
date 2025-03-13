#include "logger.h"

Logger::Logger(const std::string& filename) : file(filename, std::ios::trunc), logging(true) {
    if (!file) throw std::runtime_error("Unable to open log file");
    thread = std::thread(&Logger::loggingLoop, this);
}

Logger::~Logger() {
    logging.store(false);
    cv.notify_all();

    if (thread.joinable()) {
        thread.join();
    }

    file.close();
}

void Logger::info(const std::string& message) { logImpl("INFO", message); }
void Logger::warning(const std::string& message) { logImpl("WARNING", message); }
void Logger::error(const std::string& message) { logImpl("ERROR", message); }

void Logger::logImpl(const std::string& level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mtx);
    std::string logMessage = "[" + getCurrentTime() + "] [" + level + "] " + message;

    queue.push(logMessage);
    cv.notify_one();
}

void Logger::loggingLoop() {
    while (logging || !queue.empty()) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !queue.empty() || !logging; });

        while (!queue.empty()) {
            std::string message = queue.front();
            queue.pop();

            file << message << std::endl;
            file.flush();

            std::cout << message << std::endl;
        }
    }
}

std::string Logger::getCurrentTime() {
    std::time_t now = std::time(nullptr);
    struct tm timeStruct;
    if (localtime_s(&timeStruct, &now) != 0) {
        return "0000-00-00 00:00:00";
    }
    
    char buf[20];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &timeStruct);
    return std::string(buf);
}