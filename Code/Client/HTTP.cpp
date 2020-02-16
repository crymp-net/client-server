/**
 * @file
 * @brief Implementation of HTTP common stuff.
 */

#include "HTTP.h"

const char *HTTP::StatusCodeToString(int code)
{
	switch (static_cast<EStatusCode>(code))
	{
		case HTTP::STATUS_OK:                    return "OK";
		case HTTP::STATUS_CREATED:               return "Created";
		case HTTP::STATUS_ACCEPTED:              return "Accepted";
		case HTTP::STATUS_NO_CONTENT:            return "No Content";

		case HTTP::STATUS_MOVED_PERMANENTLY:     return "Moved Permanently";
		case HTTP::STATUS_FOUND:                 return "Found";
		case HTTP::STATUS_SEE_OTHER:             return "See Other";
		case HTTP::STATUS_NOT_MODIFIED:          return "Not Modified";
		case HTTP::STATUS_TEMPORARY_REDIRECT:    return "Temporary Redirect";

		case HTTP::STATUS_BAD_REQUEST:           return "Bad Request";
		case HTTP::STATUS_UNAUTHORIZED:          return "Unauthorized";
		case HTTP::STATUS_FORBIDDEN:             return "Forbidden";
		case HTTP::STATUS_NOT_FOUND:             return "Not Found";

		case HTTP::STATUS_INTERNAL_SERVER_ERROR: return "Internal Server Error";
		case HTTP::STATUS_NOT_IMPLEMENTED:       return "Not Implemented";
		case HTTP::STATUS_BAD_GATEWAY:           return "Bad Gateway";
		case HTTP::STATUS_SERVICE_UNAVAILABLE:   return "Service Unavailable";
		case HTTP::STATUS_GATEWAY_TIMEOUT:       return "Gateway Timeout";
	}

	return "?";
}
