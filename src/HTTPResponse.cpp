#include "HTTPResponse.h"
#include <string>
#include <vector>
#include <iterator>

void HTTPResponse::parseStartLine(const std::string& line) {
	std::istringstream iss(line);
	std::vector<std::string> tokens(std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>());

	if (tokens.size() < 3) {
		throw std::invalid_argument("Invalid HTTP response line");
	}

	statusCode = std::atoi(tokens[1].c_str());
	statusText = tokens[2];
}
