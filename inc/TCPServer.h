#pragma once

#include <string>
#include "HTTPResponse.h"
#include "ServerLogger.h"

class TCPServer {
public:
	TCPServer() noexcept : port(8888) {}

	TCPServer(short int port) noexcept
		: port(port) {}

	TCPServer(TCPServer& server)
	{
		port = server.port;
	}

	void setPort(short int& val) { port = val; }
	short int getPort() const { return port; }

	void start();
	void stop();	
	void restart();

	virtual ~TCPServer() = default;

protected:
	void sendResponse(const HTTPResponse& response, int clientSocket);
	int receiveResponse(std::string& rawMessage, int socket, timeval timeout);
	int sendStringMessage(const std::string& rawMessage, int clientSocket);
private:
	short int port;
	short int serverSocket;

	enum class ServerStatus {
		STOPPED,
		RUNNING
	};

	ServerStatus status{ ServerStatus::STOPPED };
	void handleClient(int clientSocket);
	virtual void callLog(logPriority priority, const std::string message);
	virtual void handleRequest(std::string& rawRequest, int clientSocket);
	void closeConnection(int clientSocket);
	void acceptConnection();
};
