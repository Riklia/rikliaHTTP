#include "string_utils.h"
#include <chrono>
#include <ctime>
#include <string>
#include <cstring>
#include <iomanip>
#include <sstream>

std::string string_utils::strip(const std::string& str, const std::string& chars) {
    size_t start = str.find_first_not_of(chars);
    size_t end = str.find_last_not_of(chars);
    if (start == std::string::npos) {
        return "";
    }
    return str.substr(start, end - start + 1);
}

std::vector<std::string> string_utils::split(const std::string& s, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0, end = 0;
    while ((end = s.find(delimiter, start)) != std::string::npos) {
        tokens.push_back(s.substr(start, end - start));
        start = end + delimiter.length();
    }
    tokens.push_back(s.substr(start));
    return tokens;
}

void string_utils::removeSubstring(std::string& mainStr, const std::string& substr) {
    size_t pos = mainStr.find(substr);
    while (pos != std::string::npos) {
        mainStr.erase(pos, substr.length());
        pos = mainStr.find(substr);
    }
}

std::string string_utils::getNowTimestamp() {
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::string timestamp = std::ctime(&now);
    timestamp.pop_back();
    return timestamp;
}

std::string string_utils::getCurrentTimestampForFilename() {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    std::tm* timeInfo = std::localtime(&currentTime);
    std::stringstream ss;
    ss << std::put_time(timeInfo, "%Y%m%d_%H%M%S");

    return ss.str();
}

std::string string_utils::getHeaderValue(const std::string& rawMessage, const std::string& headerName) {
    size_t pos = rawMessage.find(headerName + ": ");
    if (pos != std::string::npos) {
        pos += headerName.length() + 2;

        size_t endPos = rawMessage.find("\r\n", pos);
        if (endPos != std::string::npos) {
            return rawMessage.substr(pos, endPos - pos);
        }
    }
    return "";
}

std::string string_utils::getErrorMessage(const char* prefix)
{
    std::stringstream errorMessage;
    errorMessage << prefix << ": " << strerror(errno);
    return errorMessage.str();
}

timeval string_utils::getTimevalFromSeconds(double timeInSeconds)
{
    std::chrono::duration<double> durationInSeconds(timeInSeconds);
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(durationInSeconds);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(durationInSeconds - seconds);
    struct timeval timeValue;
    timeValue.tv_sec = seconds.count();
    timeValue.tv_usec = milliseconds.count() * 1000;
    return timeValue;
}
