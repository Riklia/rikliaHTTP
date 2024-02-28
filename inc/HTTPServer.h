#pragma once

#include "HTTPRequest.h"
#include "TCPServer.h"
#include "ServerConfiguration.h"
#include "ServerLogger.h"
#include <unordered_map>

class HTTPServer : public TCPServer {
public:
	HTTPServer() = default;
	HTTPServer(const short int port) : TCPServer(port)
	{
		serverConfig.port = port;
	}
	HTTPServer(const ServerConfig& serverConfigVal) : TCPServer(serverConfigVal.port)
	{
		serverConfig = serverConfigVal;
		serverLogger = ServerLogger(serverConfig.loglevel, serverConfig.logfile, serverConfig.maxFileSize);
	}
	HTTPServer(const HTTPServer& server) : TCPServer(server.getPort())
	{
		serverConfig = server.serverConfig;
		serverLogger = server.serverLogger;
	}
	~HTTPServer() override = default;
private:
	ServerConfig serverConfig;
	ServerLogger serverLogger;
	void handleErrorCode(HTTPResponse& response, short errorCode);
	void handleRequest(std::string& rawRequest, int clientSocket) override;
	void staticFileHandler(const LocationConfig& location, const HTTPRequest& request, HTTPResponse& response);
	void proxyHandler(const LocationConfig& location, const HTTPRequest& request, HTTPResponse& response);
	LocationConfig findMatchingLocation(const std::string& url, const std::unordered_map<std::string, LocationConfig>& locations);
	HTTPResponse sendRequestAndGetResponse(const HTTPRequest& request, const std::string& sendTo);
	void continue100Handler(int clientSocket);
	void rangeHandler(const HTTPRequest& request, HTTPResponse& response);
	void callLog(logPriority priority, const std::string message) override;
};

