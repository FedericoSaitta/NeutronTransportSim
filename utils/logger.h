#pragma once

#include <iostream>
#include <fstream>
#include <mutex>
#include <string>

// Enable debug logs only if DEBUG is defined
#ifdef DEBUG
    #define DEBUG_LOG(msg) \
    do { Logger::getInstance().log(msg, __func__); } while(0)
#else
    #define DEBUG_LOG(msg) do {} while(0)
#endif

class Logger {
public:
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    void log(const std::string& message, const char* func) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!ofs_.is_open()) {
            ofs_.open("log.txt", std::ios::out | std::ios::app);
            if (!ofs_) {
                // fallback to cerr if file can't open
                std::cerr << "[ERROR] Failed to open log.txt" << std::endl;
                return;
            }
        }
        ofs_ << "[DEBUG] " << func << "(): " << message << std::endl;
    }

private:
    Logger() {
        ofs_.open("log.txt", std::ios::out); // Clears existing log.txt
        if (!ofs_) {
            std::cerr << "[ERROR] Failed to open log.txt in Logger constructor" << std::endl;
        }
        ofs_.close(); // Close it so we can reopen later
    }
    ~Logger() {
        if (ofs_.is_open()) ofs_.close();
    }
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::ofstream ofs_;
    std::mutex mutex_;
};
