#pragma once

#include <map>
#include <string>
#include <string_view>
#include <functional>

#include "Library/Error.h"

#include "HTTP.h"

struct HTTPClientResult
{
	Error error;           // client error
	std::string response;  // server response
	int code = 0;          // HTTP status code
};

struct HTTPClientRequest
{
	std::string method = "GET";
	std::string url;
	std::string data;
	std::map<std::string, std::string> headers;
	std::function<void(HTTPClientResult&)> callback;
	int timeout = 4000;
};

class HTTPClient
{
	// cached telemetry headers
	std::string m_hwid;
	std::string m_locale;
	std::string m_timezone;

	void AddTelemetryHeaders(HTTPClientRequest & request);

public:
	HTTPClient();
	~HTTPClient();

	void Request(HTTPClientRequest && request);

	void GET(const std::string_view & url, std::function<void(HTTPClientResult&)> callback);
};
