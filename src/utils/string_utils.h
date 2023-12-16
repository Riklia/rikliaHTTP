#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace string_utils {
    std::string strip(const std::string&, const std::string& chars = " \t\n\r");
    std::vector<std::string> split(const std::string& s, const std::string& delimiter);
    void removeSubstring(std::string& mainStr, const std::string& substr);
    template <typename Iterator>
    std::string join(Iterator begin, Iterator end, const std::string& delimiter) {
        std::ostringstream result;
        if (begin != end) {
            result << *begin++;
            while (begin != end) {
                result << delimiter << *begin++;
            }
        }
        return result.str();
    }
    std::string getNowTimestamp();
    std::string getCurrentTimestampForFilename();
    std::string getHeaderValue(const std::string& rawMessage, const std::string& headerName);
    std::string getErrorMessage(const char* prefix);
    timeval getTimevalFromSeconds(double seconds);
}