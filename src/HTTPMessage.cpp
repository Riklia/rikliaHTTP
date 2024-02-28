#include "HTTPMessage.h"
#include "utils/string_utils.h"
#include <iostream>
#include <vector>
#include <string>

void HTTPMessage::parseRawMessage(const std::string& rawMessage) {
	std::vector<std::string> lines = string_utils::split(rawMessage, "\r\n");
	startLine = lines[0];
	this->parseStartLine(lines[0]);
	int i = 1;
	for (; i < lines.size(); ++i) {
		if (lines[i].empty()) { break; }
		size_t colonPos = lines[i].find(':');
		if (colonPos != std::string::npos) {
			std::string key = lines[i].substr(0, colonPos);
			std::string value = lines[i].substr(colonPos + 2);
			headers[key] = value;
		}
	}
	++i;
	if (i < lines.size()) {
		body = string_utils::join(lines.begin() + i, lines.end(), "\r\n");
	}
}

std::string HTTPMessage::toString() const {
	std::string message;
	message += startLine + "\r\n";
	for (auto it = headers.rbegin(); it != headers.rend(); ++it) {
		message += it->first + ": " + it->second + "\r\n";
	}

	message += "\r\n";
	message += body;
	return message;
}

void HTTPMessage::removeHeader(const std::string& key)
{
	auto it = headers.find(key);
	if (it != headers.end()) {
		headers.erase(it);
	}
}
