#include "ServerLogger.h"
#include "utils/string_utils.h"
#include "utils/file_utils.h"
#include <iostream>
#include <fstream>
#include <filesystem>


logPriority ServerLogger::getPriorityByString(std::string priority)
{
    const std::string priorityValues[] = { "trace", "debug", "info", "warn", "error", "fatal" };
    int pos = -1;
    for (int i = 0; i < priorityValues->size(); ++i)
    {
        if (priorityValues[i] == priority)
        {
            pos = i;
            break;
        }
    }
    if (pos == -1)
    {
        return ErrorP;
    }
    else
    {
        return static_cast<logPriority>(pos);
    }
}


void ServerLogger::log(logPriority priority, std::string message)
{
    if (priority >= verbosity) {
        if (logfile.empty())
        {
            // to stdout
            switch (priority) {
            case TraceP: std::cout << string_utils::getNowTimestamp() << " [Trace]\t"; break;
            case DebugP: std::cout << string_utils::getNowTimestamp() << " [Debug]\t"; break;
            case InfoP: std::cout << string_utils::getNowTimestamp() << " [Info]\t"; break;
            case WarnP: std::cout << string_utils::getNowTimestamp() << " [Warn]\t"; break;
            case ErrorP: std::cout << string_utils::getNowTimestamp() << " [Error]\t"; break;
            case FatalP: std::cout << string_utils::getNowTimestamp() << " [Critical]\t"; break;
            }
            std::cout << message << "\n";
        }
        else
        {
            // to file
            std::string logDirectory = std::filesystem::path(logfile).parent_path().string();
            file_utils::createDirectory(logDirectory);

            std::uintmax_t size = file_utils::getFileSize(logfile);
            if (size >= maxFileSize)
            {

                std::string newLogFileName = file_utils::getDirectory(logfile) + "/" + file_utils::getFilenameWithoutExtension(logfile);
                std::string logFileExtension = file_utils::getExtension(logfile);
                newLogFileName += "_" + string_utils::getCurrentTimestampForFilename() + "." + logFileExtension;

                std::error_code checkError;
                std::filesystem::rename(logfile, newLogFileName, checkError);

				if (checkError)
                {
                    std::cout << "Could not rename log file - continue writing to the file";
                    std::cout << "Error details: " << checkError.message();
                }
                else
                {
                    file_utils::compressFileToGz(newLogFileName);
                    file_utils::deleteFile(newLogFileName);
                    std::ofstream newFile(logfile, std::ios_base::app);
                    newFile.close();
                }
            }
            std::ofstream file(logfile, std::ios_base::app);
            switch (priority) {
            case TraceP: file << string_utils::getNowTimestamp() << " [Trace]\t"; break;
            case DebugP: file << string_utils::getNowTimestamp() << " [Debug]\t"; break;
            case InfoP: file << string_utils::getNowTimestamp() << " [Info]\t"; break;
            case WarnP: file << string_utils::getNowTimestamp() << " [Warn]\t"; break;
            case ErrorP: file << string_utils::getNowTimestamp() << " [Error]\t"; break;
            case FatalP: file << string_utils::getNowTimestamp() << " [Critical]\t"; break;
            }
            file << message << "\n";
            file.close();
        }
    }
}