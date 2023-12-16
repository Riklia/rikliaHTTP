#include "TCPServer.h"
#include "HTTPRequest.h"
#include "ServerLogger.h"
#include "utils/string_utils.h"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <thread>
#include <mutex>
#include <cstring>


#define BUFFER_SIZE 30000

void TCPServer::start() {
	if (status == ServerStatus::RUNNING) {
		callLog(WarnP, "The server is already running");
		return;
	}
	callLog(InfoP, "Starting server...");
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	status = ServerStatus::RUNNING;
	sockaddr_in addr;
	int opt = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int bindResult = bind(serverSocket, (struct sockaddr*)&addr, sizeof(addr));
	if (bindResult == -1) 
	{
		std::string errorMessage = string_utils::getErrorMessage("Bind failed");
		callLog(FatalP, errorMessage);
	}
	int listenResult = listen(serverSocket, 100);
	if (listenResult == -1) 
	{
		std::string errorMessage = string_utils::getErrorMessage("Listen failed");
		callLog(FatalP, errorMessage);
	}
	callLog(InfoP, "Server has started");
	while (true) {
		acceptConnection();
	}
}

void TCPServer::acceptConnection() {
	sockaddr_in clientAddr;
	socklen_t clientAddrSize = sizeof(clientAddr);

	while (true) {
		int clientSocket = accept(serverSocket, nullptr, nullptr);
		if (clientSocket == -1) {
			continue;
		}
		std::thread([this, clientSocket]() {
			handleClient(clientSocket);
			closeConnection(clientSocket);
			}).detach();
	}
}

void TCPServer::closeConnection(int clientSocket) {
	close(clientSocket);
}

void TCPServer::handleClient(int clientSocket) {
	char buffer[BUFFER_SIZE];
	int bytesRead = read(clientSocket, buffer, BUFFER_SIZE - 1);
	std::string rawRequest = buffer;
	std::string debugMessage = "Received request: \n" + rawRequest;
	callLog(DebugP, debugMessage);
	handleRequest(rawRequest, clientSocket);
}

int TCPServer::sendStringMessage(const std::string& rawMessage, int clientSocket) {
	int bytesSent = send(clientSocket, rawMessage.c_str(), rawMessage.size(), 0);

	if (bytesSent == -1)
	{
		std::string errorMessage = string_utils::getErrorMessage("Error sending message");
		callLog(ErrorP, errorMessage);
	}
	else
	{
		std::string debugMessage = "Sent message: \n" + rawMessage;
		callLog(DebugP, debugMessage);
	}
	return bytesSent;
}

int TCPServer::receiveResponse(std::string& rawMessage, int socket, timeval timeout) {
	char buffer[BUFFER_SIZE];
	ssize_t bytesRead;

	if (setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) 
	{
		std::string errorMessage = string_utils::getErrorMessage("Error setting socket timeout");
		callLog(ErrorP, errorMessage);
		return -1;
	}

	while ((bytesRead = recv(socket, buffer, sizeof(buffer) - 1, 0)) > 0) {
		buffer[bytesRead] = '\0';
		rawMessage += buffer;
		size_t contentLength = std::atoi(string_utils::getHeaderValue(rawMessage, "Content-Length").c_str());
		if (contentLength > 0 && rawMessage.length() >= contentLength) {
			break;
		}
	}

	if (bytesRead < 0) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			callLog(WarnP, "Socket receive timeout");
			return 1;
		}
		else {
			std::string errorMessage = string_utils::getErrorMessage("Error receiving response");
			callLog(ErrorP, errorMessage);
			return -1;
		}
	}

	return 0;
}

void TCPServer::sendResponse(const HTTPResponse& response, int clientSocket) {
	callLog(DebugP, "Sending response to client");
	sendStringMessage(response.toString(), clientSocket);
}


void TCPServer::handleRequest(std::string& rawRequest, int clientSocket)
{
	sendStringMessage(rawRequest, clientSocket);
}

void TCPServer::restart()
{
	callLog(InfoP, "Restarting server");
	stop();
	start();
}

void TCPServer::callLog(logPriority priority, const std::string message)
{
	switch (priority)
	{
	case TraceP: std::cout << string_utils::getNowTimestamp() << "[Trace]\t"; break;
	case DebugP: std::cout << string_utils::getNowTimestamp() << "[Debug]\t"; break;
	case InfoP: std::cout << string_utils::getNowTimestamp() << "[Info]\t"; break;
	case WarnP: std::cout << string_utils::getNowTimestamp() << "[Warn]\t"; break;
	case ErrorP: std::cout << string_utils::getNowTimestamp() << "[Error]\t"; break;
	case FatalP: std::cout << string_utils::getNowTimestamp() << "[Critical]\t"; break;
	}
	std::cout << message << "\n";
}

void TCPServer::stop()
{
	if (serverSocket) {
		close(serverSocket);
		status = ServerStatus::STOPPED;
		serverSocket = 0;
		callLog(InfoP, "Server stopped");
	}
}
