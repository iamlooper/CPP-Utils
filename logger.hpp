#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>

// Use standard namespace.
using namespace std;

// Enumeration for different types of log messages.
enum class LogType {
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    // Constructor to initialize log type and stream.
    Logger(LogType type, ostream& logStream = cout) : logType(type), logStream(logStream) {
        // Initialize log with timestamp and log type.
        logStream << "[" << getTimestamp() << "] ";
        logStream << "[" << logTypeToString(type) << "] ";
    }

    // Overload stream insertion operator for Logger.
    template <typename T>
    Logger& operator<<(const T& value) {
        // Log values.
        logStream << value;
        return *this;
    }

    // Overload stream insertion operator for manipulators.
    Logger& operator<<(ostream& (*manip)(ostream&)) {
        // Log manipulators.
        logStream << manip;
        return *this;
    }

private:
    // Get current time as a string.
    static string getTimestamp() {
        time_t current_time = time(nullptr);
        stringstream date;
        date << put_time(localtime(&current_time), "%Y-%m-%d %H:%M:%S");
        return date.str();
    }

    // Convert LogType enum to corresponding string.
    static string logTypeToString(LogType type) {
        switch (type) {
            case LogType::INFO:
                return "Info";
            case LogType::WARNING:
                return "Warning";
            case LogType::ERROR:
                return "Error";
            default:
                return "Unknown";
        }
    }

    // Log type and stream member variables.
    LogType logType;
    ostream& logStream;
};