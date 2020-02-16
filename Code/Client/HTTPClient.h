/**
 * @file
 * @brief HTTP client.
 */

#pragma once

#include <string>

#include "HTTP.h"

struct HTTPClient
{
	typedef void (*TCallback)(int status, const std::string & result, void *param);

	void GET(const char *url, TCallback pCallback, void *param);
	void POST(const char *url, const char *data, const char *contentType, TCallback pCallback, void *param);
};
