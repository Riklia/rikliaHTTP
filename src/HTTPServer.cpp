#include <unordered_map>
#include <regex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "utils/file_utils.h"
#include "HTTPServer.h"
#include "HTTPRequest.h"
#include "GlobalConfigurations.h"
#include "utils/string_utils.h"
#include "ServerLogger.h"

void HTTPServer::handleRequest(std::string& rawRequest, int clientSocket) {
    HTTPResponse response;
    HTTPRequest request;
    try
    {
        request = HTTPRequest(rawRequest);
    }
    catch (std::invalid_argument)
    {
        handleErrorCode(response, 400);
        sendResponse(response, clientSocket);
        return;
    }
    std::string requestedUrl = request.getUrl();
    LocationConfig location = findMatchingLocation(requestedUrl, serverConfig.locations);
    if (location.path.empty())
    {
        handleErrorCode(response, 404);
    }
    else {
        bool isProxy = location.isProxy;
        response.addHeader("Server", serverConfig.serverName);
        if (!isProxy)
        {
            callLog(DebugP, "Calling static file handler for " + location.path);
            if (request.getHeader("Expect") == "100-continue")
            {
                continue100Handler(clientSocket);
            }
            staticFileHandler(location, request, response);
            rangeHandler(request, response);
            response.addHeader("Content-Length", std::to_string(response.getBody().size()));
        }
        else
        {
            callLog(DebugP, "Calling proxy handler for " + location.path);
            proxyHandler(location, request, response);
        }
    }
    if (response.getStatusCode() == 0)
    {
        handleErrorCode(response, 404);
    }
    sendResponse(response, clientSocket);
}

HTTPResponse HTTPServer::sendRequestAndGetResponse(const HTTPRequest& request, const std::string& sendTo) {
    std::vector<std::string> destinationInfo = string_utils::split(sendTo, ":");
	HTTPResponse response;
    if (destinationInfo.size() == 1) {
        destinationInfo.push_back("80");
    }

    if (destinationInfo.size() != 2) {
        handleErrorCode(response, 400);
        return response;
    }

    const char* sendToIP = destinationInfo[0].c_str();
    const short int sendToPort = std::atoi(destinationInfo[1].c_str());

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Error creating socket");
        handleErrorCode(response, 500);
        return response;
    }

    struct sockaddr_in serverAddress;
    struct hostent* server = gethostbyname(sendToIP);
    if (!server) {
        std::string errorMessage = string_utils::getErrorMessage("Error resolving host");
    	callLog(ErrorP, errorMessage);
        close(clientSocket);
        handleErrorCode(response, 500);
        return response;
    }

    bzero((char*)&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    bcopy((char*)server->h_addr, (char*)&serverAddress.sin_addr.s_addr, server->h_length);
    serverAddress.sin_port = htons(sendToPort);

    if (connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        std::string errorMessage = string_utils::getErrorMessage("Connection failed");
        callLog(ErrorP, errorMessage);
        close(clientSocket);
        handleErrorCode(response, 500);
        return response;
    }

    callLog(DebugP, "Sending request to " + sendTo);
    std::string rawRequest = request.toString();
    if (sendStringMessage(rawRequest, clientSocket) < 0) {
        close(clientSocket);
        handleErrorCode(response, 500);
        return response;
    }

    std::string rawResponse;
    callLog(DebugP, "Getting response from " + sendTo);
    struct timeval timeout = serverConfig.proxyConnectTimeout;
    int receiveResult = receiveResponse(rawResponse, clientSocket, timeout);
    if (receiveResult < 0) 
    {
        close(clientSocket);
        handleErrorCode(response, 500);
        return response;
    }
    if (receiveResult == 1)
    {
        close(clientSocket);
        handleErrorCode(response, 408);
        return response;
    }

    close(clientSocket);
    response = HTTPResponse(rawResponse);
    return response;
}

LocationConfig HTTPServer::findMatchingLocation(const std::string& url, const std::unordered_map<std::string, LocationConfig>& locations) {
    LocationConfig bestMatchConfig;
    size_t bestMatchLength = 0;
    for (const auto& location : locations) {
        const std::string& pattern = location.first;
        std::regex regexPattern(pattern);

        if (std::regex_search(url, regexPattern)) {
            if (pattern.length() > bestMatchLength) {
                bestMatchConfig = location.second;
                bestMatchLength = pattern.length();
            }
        }
    }
    return bestMatchConfig;
}

void HTTPServer::handleErrorCode(HTTPResponse& response, short int errorCode)
{
    std::string errorPagesDir;
    if(errorCode < 400 || errorCode >= 600)
    {
        return;
    }
    if (errorCode < 500)
    {
		errorPagesDir = serverConfig.errorPages4xx;
    }
    else
    {
        errorPagesDir = serverConfig.errorPages5xx;
    }
    response.setStatusCode(errorCode);
    response.addHeader("Content-Type", "text/html");
    std::string errorFile = file_utils::getFile(errorPagesDir, std::to_string(errorCode) + ".html");
    if (file_utils::fileExists(errorFile))
    {
        std::ifstream errorPage(errorFile);
        std::stringstream content;
        content << errorPage.rdbuf();
        response.setBody(content.str());
    }
    else
    {
        response.setBody("<h1> " + std::to_string(response.getStatusCode()) + " " + response.getStatusText() + "</h1>");
    }
    response.addHeader("Content-Length", std::to_string(response.getBody().size()));
}

void HTTPServer::staticFileHandler(const LocationConfig& location, const HTTPRequest& request, HTTPResponse& response)
{
    std::string filename = request.getUrl();
    string_utils::removeSubstring(filename, location.path);
    const std::string& method = request.getMethod();
    if (method != "GET")
    {
        handleErrorCode(response, 405);
        return;
    }
    if (filename.empty())
    {
        filename = location.index;
    }
    filename = file_utils::getFile(location.rootDir, filename);
    if (!file_utils::fileExists(filename))
    {
        handleErrorCode(response, 404);
        return;
    }

    std::ifstream requestedFile(filename, std::ios::binary);
    if (requestedFile.fail()) 
    {
        std::string errorMessage = "Error reading file: " + filename;
        callLog(ErrorP, errorMessage);
        handleErrorCode(response, 500);
    }
    requestedFile.seekg(0, std::ios::beg);
    std::stringstream content;
    content << requestedFile.rdbuf();
    response.setBody(content.str());

    response.setStatusCode(200);
    const auto& globalConfigs = GlobalConfigurations::instance();
    response.addHeader("Content-Type", globalConfigs->guessMimeType(filename));
}

void HTTPServer::proxyHandler(const LocationConfig& location, const HTTPRequest& request, HTTPResponse& response)
{
    const std::string destination = location.proxyPass;
    std::string page = request.getUrl();
    string_utils::removeSubstring(page, location.path);
    const HTTPResponse receivedResponse = sendRequestAndGetResponse(request, destination);
    for (auto& header : receivedResponse.getHeaders())
    {
        response.addHeader(header.first, header.second);
    }
    response.setStatusCode(receivedResponse.getStatusCode());
    if (receivedResponse.getStatusCode() >= 400 && receivedResponse.getStatusCode() <= 600)
    {
        handleErrorCode(response, response.getStatusCode());
    }
    else
    {
		response.setBody(receivedResponse.getBody());
    }
}

void HTTPServer::continue100Handler(int clientSocket)
{
    callLog(DebugP, "Handling Expected: 100-continue");
    HTTPResponse response;
    response.setStatusCode(100);
    sendResponse(response, clientSocket);
}

void HTTPServer::rangeHandler(const HTTPRequest& request, HTTPResponse& response)
{
    std::string rangeHeader = request.getHeader("Range");
    if (rangeHeader.empty()) return;
    const size_t posDash = rangeHeader.find('-');
    if (posDash == std::string::npos)
    {
        handleErrorCode(response, 416);
        return;
    }
    size_t bytesPrefixPos = rangeHeader.find("bytes=");
    if (bytesPrefixPos == std::string::npos) 
    {
        handleErrorCode(response, 416);
        return;
    }

    std::string startStr = rangeHeader.substr(6, posDash - 6);
    std::string endStr = rangeHeader.substr(posDash + 1);

    try
    {
        const std::string responseBody = response.getBody();
        size_t start, end;
        if (startStr.empty() && !endStr.empty())
        {
            end = std::stoull(endStr);
            start = (end >= responseBody.size()) ? 0 : responseBody.size() - end;

            startStr = std::to_string(start);
        }
        start = std::stoull(startStr);
        end = (endStr.empty()) ? 0 : std::stoull(endStr);

        end = (end == 0 || end >= responseBody.size()) ? responseBody.size() - 1 : end;

        std::ostringstream contentRange;
        contentRange << "bytes " << start << "-" << end << "/" << responseBody.size();
        response.addHeader("Content-Range", contentRange.str());

        response.setStatusCode(206);
        const size_t contentLength = end - start + 1;
        const std::string partialContent = responseBody.substr(start, contentLength);
        response.setBody(partialContent);
    }
    catch (const std::exception& e)
    {
        std::cout << "Error parsing range values: " << e.what() << std::endl;
        handleErrorCode(response, 416);
    }
}

void HTTPServer::callLog(logPriority priority, const std::string message)
{
    serverLogger.log(priority, message);
}

