#include "HTTPRequest.h"
#include <string>
#include <iterator>
#include <vector>
#include <iostream>

void HTTPRequest::parseStartLine(const std::string& line) {
	std::istringstream iss(line);
	std::vector<std::string> tokens(std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>());
	if (tokens.size() < 3) {
		throw std::invalid_argument("Invalid HTTP request line");
	}

	method = tokens[0];
	url = tokens[1];
}
