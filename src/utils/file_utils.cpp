#include "file_utils.h"
#include "string_utils.h"
#include <sys/stat.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <filesystem>
#include <cstdio>
#include <cstring>
#include <zlib.h>

bool file_utils::fileExists(const std::string& name) {
    struct stat buffer;
    return (stat(name.c_str(), &buffer) == 0);
}

void file_utils::createDirectory(const std::string& path) {
    try {
        std::filesystem::create_directories(path);
    }
    catch (const std::exception& e) {
        std::cout << "Error creating directory: " << e.what() << std::endl;
    }
}

std::string file_utils::getCurrentDirectory() {
    try {
        return std::filesystem::current_path().string();
    }
    catch (const std::exception& e) {
        std::cout << "Error getting current directory: " << e.what() << std::endl;
        return "";
    }
}

std::unordered_map<std::string, std::string> file_utils::parseFile(const std::string& filename, bool backward, const std::string& eof) {
    std::unordered_map<std::string, std::string> result;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Failed to open file: " << filename << std::endl;
        return result;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = string_utils::strip(line);
        string_utils::removeSubstring(line, eof);
        if (backward) {
            std::vector<std::string> tokens = string_utils::split(line, " ");
            for (int i = 1; i < tokens.size(); ++i) {
                result[tokens[i]] = tokens[0];
            }
        }
        else {
            std::vector<std::string> tokens = string_utils::split(line, " ");
            if (tokens.size() >= 2) {
                result[tokens[0]] = tokens[1];
            }
        }
    }

    file.close();
    return result;
}

std::string file_utils::getExtension(std::string file) {
    return string_utils::split(file, ".").back();
}

std::string file_utils::getFile(const std::string& dir, const std::string& filename)
{
    return dir + "/" + filename;
}

std::uintmax_t file_utils::getFileSize(const std::string& file)
{
    if (std::filesystem::exists(file) && std::filesystem::is_regular_file(file)) {
        return std::filesystem::file_size(file) / 1024; // in KBs
    } 
    return 0;
}

std::string file_utils::getFilename(const std::string& filePath) {
    std::filesystem::path pathObj(filePath);
    return pathObj.filename().string();
}

std::string file_utils::getFilenameWithoutExtension(const std::string& filePath) {
    std::filesystem::path pathObj(filePath);
    return pathObj.stem().string();
}

std::string file_utils::getDirectory(const std::string& filePath) {
    std::filesystem::path pathObj(filePath);
    return pathObj.parent_path().string();
}

bool file_utils::compressFileToGz(const std::string& filePath, const std::string& outputName)
{
    std::ifstream inputFile(filePath, std::ios::binary);
    std::ofstream outputFile(outputName, std::ios::binary);

    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE];

    z_stream zs;
    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;

    deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 31, 8, Z_DEFAULT_STRATEGY);

    int flush;
    do {
        inputFile.read(buffer, BUFFER_SIZE);
        zs.avail_in = inputFile.gcount();
        flush = inputFile.eof() ? Z_FINISH : Z_NO_FLUSH;
        zs.next_in = reinterpret_cast<Bytef*>(buffer);

        do {
            char outbuffer[BUFFER_SIZE];
            zs.avail_out = BUFFER_SIZE;
            zs.next_out = reinterpret_cast<Bytef*>(outbuffer);

            deflate(&zs, flush);

            int have = BUFFER_SIZE - zs.avail_out;
            outputFile.write(outbuffer, have);
        } while (zs.avail_out == 0);
    } while (flush != Z_FINISH);

    deflateEnd(&zs);

    inputFile.close();
    outputFile.close();

    return true;
}

bool file_utils::compressFileToGz(const std::string& filePath)
{
    std::string compressedFilePath = filePath + ".gz";
    return compressFileToGz(filePath, compressedFilePath);
}

bool file_utils::deleteFile(std::string filePath)
{
    if (remove(filePath.c_str()) != 0) 
    {
        perror("Error deleting file");
        return false;
    }
	return true;
    
}

