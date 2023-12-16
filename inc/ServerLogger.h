#pragma once

#include <string>

enum logPriority {
    TraceP, DebugP, InfoP, WarnP, ErrorP, FatalP
};

class ServerLogger {
public:
    ServerLogger() = default;
    ServerLogger(ServerLogger& sLogger) = default;
    ServerLogger(std::string priority, std::string filename, std::uintmax_t maxLogFileSize)
    {
        verbosity = getPriorityByString(priority);
        logfile = filename;
        maxFileSize = maxLogFileSize;
    }
	static logPriority getPriorityByString(std::string priority);
    void setVerbosity(const std::string verbosityVal)
    {
        verbosity = getPriorityByString(verbosityVal);
    }
    void setVerbosity(const logPriority verbosityVal) { verbosity = verbosityVal; }
    logPriority getVerbosity() const { return verbosity; }
    void setLogfile(const std::string logfileVal) { logfile = logfileVal; }
    std::string getLogfile() const { return logfile; }
    void setMaxFileSize(const int maxFilesizeVal) { maxFileSize = maxFilesizeVal; }
    std::uintmax_t getMaxFileSize() { return maxFileSize; }
    void log(logPriority priority, std::string message);
private:
    logPriority verbosity = logPriority::ErrorP;
    std::string logfile;
    std::uintmax_t maxFileSize;
};