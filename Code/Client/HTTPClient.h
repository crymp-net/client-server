#pragma once

#include <string>
#include <string_view>
#include <functional>
#include <map>

#include "Library/Error.h"

#include "HTTP.h"

class HTTPClient
{
public:
	struct Result
	{
		int code = 0;          // HTTP status code
		Error error;           // client error
		std::string response;  // server response
	};

	using Callback = std::function<void(Result&)>;

	static constexpr int DEFAULT_TIMEOUT = 4000;

	void Request(
		const std::string_view & method,
		const std::string_view & url,
		const std::string_view & data,
		std::map<std::string, std::string> && headers,
		Callback callback,
		int timeout = DEFAULT_TIMEOUT
	);

	void GET(
		const std::string_view & url,
		Callback callback,
		int timeout = DEFAULT_TIMEOUT
	);

	void POST(
		const std::string_view & url,
		const std::string_view & data,
		const std::string_view & contentType,
		Callback callback,
		int timeout = DEFAULT_TIMEOUT
	);
};
