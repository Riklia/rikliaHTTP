#pragma once

#include <string>
#include <unordered_map>

namespace file_utils {
    bool fileExists(const std::string& name);
    std::string getCurrentDirectory();
    std::unordered_map<std::string, std::string> parseFile(const std::string&, bool backward = false, const std::string& eof = ";");
    std::string getExtension(std::string);
    std::string getFilename(const std::string& filePath);
    std::string getFilenameWithoutExtension(const std::string& filePath);
    std::string getFile(const std::string& dir, const std::string& filename);
    std::string getDirectory(const std::string& filePath);
    void createDirectory(const std::string& path);
    std::uintmax_t getFileSize(const std::string& file);
    bool compressFileToGz(const std::string& filePath, const std::string& outputName);
    bool compressFileToGz(const std::string& filePath);
    bool deleteFile(std::string filePath);
}