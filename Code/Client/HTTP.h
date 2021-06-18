#pragma once

#include <string>
#include <string_view>

namespace HTTP
{
	enum EStatusCode
	{
		STATUS_OK                    = 200,
		STATUS_CREATED               = 201,
		STATUS_ACCEPTED              = 202,
		STATUS_NO_CONTENT            = 204,

		STATUS_MOVED_PERMANENTLY     = 301,
		STATUS_FOUND                 = 302,
		STATUS_SEE_OTHER             = 303,
		STATUS_NOT_MODIFIED          = 304,
		STATUS_TEMPORARY_REDIRECT    = 307,

		STATUS_BAD_REQUEST           = 400,
		STATUS_UNAUTHORIZED          = 401,
		STATUS_FORBIDDEN             = 403,
		STATUS_NOT_FOUND             = 404,

		STATUS_INTERNAL_SERVER_ERROR = 500,
		STATUS_NOT_IMPLEMENTED       = 501,
		STATUS_BAD_GATEWAY           = 502,
		STATUS_SERVICE_UNAVAILABLE   = 503,
		STATUS_GATEWAY_TIMEOUT       = 504,
	};

	const char *StatusCodeToString(int code);

	std::string URLEncode(const std::string_view & text);
}
