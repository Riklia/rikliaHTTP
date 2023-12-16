#pragma once

#include <string>
#include <unordered_map>
#include <fstream>


struct LocationConfig {
	std::string path;
	bool isProxy = 0;
	std::string index;
	std::string rootDir;
	std::string proxyPass;
};

struct ServerConfig {
	short int port = 8888;
	std::string serverName = "server";
	std::string errorPages4xx; // error_pages_4xx
	std::string errorPages5xx; // error_pages_5xx
    std::string logfile;
	std::string loglevel;
	timeval proxyConnectTimeout{3, 0}; // proxy_connect_timeout
	std::uintmax_t maxFileSize = 1024 * 1024; // maxsize_logfile
	std::unordered_map<std::string, LocationConfig> locations;
	static std::unordered_map<std::string, ServerConfig> parseConfigurationFile(const std::string& filename);
};