#pragma once

#include "HTTPMessage.h"
#include <string>
#include <sstream>

class HTTPRequest: public HTTPMessage {
public:
	HTTPRequest() : HTTPMessage() {
		method = "";
		url = "";
	}
	HTTPRequest(std::map<std::string, std::string>& headersVal, 
		const std::string& bodyVal, const std::string& methodVal, const std::string& urlVal) : HTTPMessage("", headersVal, bodyVal) {
		method = methodVal;
		url = urlVal;
		setStartLine(method + " " + url + " " + httpVersion);
	}
	HTTPRequest(const HTTPRequest& httpRequest) {
		headers = httpRequest.headers;
		body = httpRequest.body;
		method = httpRequest.method;
		url = httpRequest.url;
	}
	HTTPRequest(const std::string& rawMessage) {
		parseRawMessage(rawMessage);
	};
	
	void setMethod(const std::string& val) { method = val; }
	std::string getMethod() const { return method; }
	void setUrl(const std::string& val) { url = val; }
	std::string getUrl() const { return url; }

	void parseStartLine(const std::string& line) override;
private:
	std::string method;
	std::string url;

};