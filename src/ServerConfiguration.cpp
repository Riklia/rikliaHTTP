#include <sstream>
#include <iostream>
#include "utils/file_utils.h"
#include "utils/string_utils.h"
#include "ServerConfiguration.h"


std::unordered_map<std::string, ServerConfig> ServerConfig::parseConfigurationFile(const std::string& filename) {
    if (!file_utils::fileExists(filename)) {
        std::cout << filename << " does not exist.";
        exit(1);
    }
    std::ifstream configFile(filename);
    std::string line;
    std::unordered_map<std::string, ServerConfig> servers;
    ServerConfig currentServer;
    LocationConfig currentLocation;

    while (std::getline(configFile, line)) {
        std::istringstream iss(line);
        std::string keyword;
        iss >> keyword;

        if (keyword == "server") {
            iss >> keyword;
            currentServer = {};
        }
        else if (keyword == "listen") {
            iss >> currentServer.port;
        }
        else if (keyword == "server_name") {
            iss >> currentServer.serverName;
        }
        else if (keyword == "error_pages_4xx") {
            iss >> currentServer.errorPages4xx;
        }
        else if (keyword == "error_pages_5xx") {
            iss >> currentServer.errorPages5xx;
        }
        else if (keyword == "logfile") {
            iss >> currentServer.logfile;
        }
        else if (keyword == "loglevel") {
            iss >> currentServer.loglevel;
        }
        else if (keyword == "maxsize_logfile")
        {
            std::string maxFileSizeStr;
            iss >> maxFileSizeStr;
            int maxSize = std::stoi(maxFileSizeStr);
            if (maxSize > 0) currentServer.maxFileSize = maxSize;
        }
        else if (keyword == "location") 
        {
            currentLocation = {};
            iss >> currentLocation.path;
        }
        else if (keyword == "proxy_connect_timeout")
        {
            std::string timeValueStr;
            iss >> timeValueStr;
            try
            {
				currentServer.proxyConnectTimeout = string_utils::getTimevalFromSeconds(std::stod(timeValueStr));
            }
            catch (const std::invalid_argument& e) 
            {
                std::cout << "Invalid argument: " << e.what() << std::endl;
            }
        }
        else if (keyword == "{") {}
        else if (keyword == "}") {
            if (!currentServer.serverName.empty()) {
                servers[currentServer.serverName] = currentServer;
            }
            if (!currentLocation.path.empty()) {
                currentServer.locations[currentLocation.path] = currentLocation;
            }
        }
        else if (keyword == "index") {
            iss >> currentLocation.index;
        }
        else if (keyword == "root") {
            iss >> currentLocation.rootDir;
        }
        else if (keyword == "proxy_pass") {
            iss >> currentLocation.proxyPass;
            currentLocation.isProxy = true;
        }
    }
    return servers;
}