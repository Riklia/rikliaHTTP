#pragma once

#include <string>
#include <map>

class HTTPMessage {
public:
    HTTPMessage() : startLine(""), headers({}), body("") {};
    HTTPMessage(std::string rawMessage) {
        parseRawMessage(rawMessage);
    }
    HTTPMessage(std::string startLineVal, std::map<std::string, std::string>& headersVal, std::string bodyVal) {
        startLine = startLineVal;
        headers = headersVal;
        body = bodyVal;
    };
    HTTPMessage(HTTPMessage& message) {
        startLine = message.startLine;
        headers = message.headers;
        body = message.body;
    };
    void setStartLine(const std::string& val) { startLine = val; }
    std::string getStartLine() const { return startLine; }

    void addHeader(const std::string& key, const std::string& value) { headers[key] = value; }
    std::string getHeader(const std::string& key) const
	{
        auto it = headers.find(key);
        return (it != headers.end()) ? it->second : "";
    }
    std::map<std::string, std::string> getHeaders() const
    {
        return headers;
    }

    void setHTTPVersion(const std::string val) { httpVersion = val; }
    std::string getHTTPVersion() { return httpVersion; }
    
    void setBody(const std::string& val) { body = val; }
    std::string getBody() const { return body; }

    std::string toString() const;
    virtual ~HTTPMessage() = default;
protected:
    void parseRawMessage(const std::string& rawMessage);
    std::string startLine;
    std::map<std::string, std::string> headers;
    std::string body;
    std::string httpVersion = "HTTP/1.1";
    virtual void parseStartLine(const std::string& line) = 0;
};