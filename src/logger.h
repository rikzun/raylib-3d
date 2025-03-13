#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <ctime>
#include <thread>
#include <queue>
#include <condition_variable>
#include <atomic>

class Logger {
    public:
        Logger(const std::string& filename);
        ~Logger();

        void info(const std::string& message);
        void warning(const std::string& message);
        void error(const std::string& message);

    private:
        std::ofstream file;
        std::mutex mtx;
        std::condition_variable cv;
        std::queue<std::string> queue;
        std::thread thread;
        std::atomic<bool> logging;

        void logImpl(const std::string& level, const std::string& message);
        void loggingLoop();
        std::string getCurrentTime();
};