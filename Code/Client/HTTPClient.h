#pragma once

#include <string>
#include <map>
#include <string_view>
#include <functional>

#include "Library/Error.h"

#include "HTTP.h"

class HTTPClient
{
public:
	using Callback = std::function<void(int, std::string&, Error&)>;  // status code, server reply, client error

	void GET(const std::string_view & url, Callback callback, int timeout = 4000);

	void POST(const std::string_view & url, const std::string_view & data,
	          const std::string_view & contentType, Callback callback, int timeout = 4000);

	void Request(
		const std::string_view& method,
		const std::string_view& url,
		const std::map<std::string, std::string>& headers,
		const std::string_view& body,
		Callback callback,
		int timeout = 4000,
		bool cache = false,
		bool returnPath = false
	);
};
