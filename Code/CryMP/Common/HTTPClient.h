#pragma once

#include <map>
#include <string>
#include <string_view>
#include <functional>

#include "Library/Error.h"

#include "HTTP.h"

class Executor;

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
	Executor* m_executor = nullptr;

public:
	explicit HTTPClient(Executor& executor);
	~HTTPClient();

	void Request(HTTPClientRequest&& request);
};
