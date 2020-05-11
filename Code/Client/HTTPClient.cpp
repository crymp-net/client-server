/**
 * @file
 * @brief Implementation of HTTP client.
 */

#include <cstring>
#include <new>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>

#include "CryCommon/ISystem.h"
#include "Launcher/Launcher.h"
#include "Launcher/IExecutorTask.h"

#include "HTTPClient.h"

#define HTTP_CLIENT_USER_AGENT L"CryMP-Client"
#define HTTP_CLIENT_TIMEOUT_MS 5000

static std::wstring ConvertUTF8To16(const char *input)
{
	if (!input)
	{
		return std::wstring();
	}

	std::wstring output;

	wchar_t buffer[2048];
	const int bufferLength = sizeof buffer / sizeof (wchar_t);

	// get length of entire input string, including terminating null character
	int remainingLength = MultiByteToWideChar(CP_UTF8, 0, input, -1, buffer, 0);
	if (remainingLength > 0)
	{
		// skip terminating null character
		remainingLength--;

		int pos = 0;
		while (remainingLength > 0)
		{
			const int chunkLength = (remainingLength > bufferLength) ? bufferLength : remainingLength;

			int convertedLength = MultiByteToWideChar(CP_UTF8, 0, input + pos, chunkLength, buffer, bufferLength);
			if (convertedLength > 0)
			{
				output.append(buffer, convertedLength);

				remainingLength -= convertedLength;
				pos += convertedLength;
			}
			else
			{
				break;
			}
		}
	}

	return output;
}

static std::wstring ConvertUTF8To16(const std::string & input)
{
	return ConvertUTF8To16(input.c_str());
}

class HTTPHandleGuard
{
	HINTERNET m_handle;

public:
	HTTPHandleGuard(HINTERNET handle)
	: m_handle(handle)
	{
	}

	~HTTPHandleGuard()
	{
		if (m_handle)
		{
			WinHttpCloseHandle(m_handle);
		}
	}

	operator HINTERNET()
	{
		return m_handle;
	}

	explicit operator bool() const
	{
		return m_handle != nullptr;
	}
};

struct HTTPClientRequestTask : public IExecutorTask
{
	enum EMethod
	{
		GET, POST
	};

	int status;
	EMethod method;
	std::string url;
	std::string data;
	std::string contentType;
	std::string result;
	HTTPClient::TCallback pCallback;
	void *callbackParam;

	// worker thread
	void execute() override;

	// main thread
	void callback() override
	{
		pCallback(status, result, callbackParam);
	}
};

void HTTPClientRequestTask::execute()
{
	status = -1;

	const std::wstring urlW = ConvertUTF8To16(url);

	URL_COMPONENTS urlComponents;
	std::memset(&urlComponents, 0, sizeof urlComponents);
	urlComponents.dwStructSize = sizeof urlComponents;

	urlComponents.dwSchemeLength = -1;
	urlComponents.dwHostNameLength = -1;
	urlComponents.dwUrlPathLength = -1;
	urlComponents.dwExtraInfoLength = -1;

	if (!WinHttpCrackUrl(urlW.c_str(), urlW.length(), 0, &urlComponents))
	{
		CryLogErrorAlways("HTTPClient: WinHttpCrackUrl error code %lu", GetLastError());
		return;
	}

	HTTPHandleGuard hSession = WinHttpOpen(HTTP_CLIENT_USER_AGENT,
	                                       WINHTTP_ACCESS_TYPE_NO_PROXY,
	                                       WINHTTP_NO_PROXY_NAME,
	                                       WINHTTP_NO_PROXY_BYPASS, 0);
	if (!hSession)
	{
		CryLogErrorAlways("HTTPClient: WinHttpOpen error code %lu", GetLastError());
		return;
	}

	if (!WinHttpSetTimeouts(hSession,
	                        HTTP_CLIENT_TIMEOUT_MS,
	                        HTTP_CLIENT_TIMEOUT_MS,
	                        HTTP_CLIENT_TIMEOUT_MS,
	                        HTTP_CLIENT_TIMEOUT_MS))
	{
		CryLogErrorAlways("HTTPClient: WinHttpSetTimeouts error code %lu", GetLastError());
		return;
	}

	std::wstring serverNameW(urlComponents.lpszHostName, urlComponents.dwHostNameLength);

	HTTPHandleGuard hConnect = WinHttpConnect(hSession, serverNameW.c_str(), urlComponents.nPort, 0);
	if (!hConnect)
	{
		CryLogErrorAlways("HTTPClient: WinHttpConnect error code %lu", GetLastError());
		return;
	}

	DWORD requestFlags = WINHTTP_FLAG_REFRESH;
	if (urlComponents.nScheme == INTERNET_SCHEME_HTTPS)
	{
		requestFlags |= WINHTTP_FLAG_SECURE;
	}

	// URL components are not null-terminated, but whole URL is, so URL path contains both path and parameters
	HTTPHandleGuard hRequest = WinHttpOpenRequest(hConnect, (method == POST) ? L"POST" : L"GET",
	                                              urlComponents.lpszUrlPath, NULL,
	                                              WINHTTP_NO_REFERER,
	                                              WINHTTP_DEFAULT_ACCEPT_TYPES, requestFlags);
	if (!hRequest)
	{
		CryLogErrorAlways("HTTPClient: WinHttpOpenRequest error code %lu", GetLastError());
		return;
	}

	if (data.empty())
	{
		if (!WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0))
		{
			CryLogErrorAlways("HTTPClient: WinHttpSendRequest error code %lu", GetLastError());
			return;
		}
	}
	else
	{
		std::wstring headersW;
		if (!contentType.empty())
		{
			headersW += L"Content-Type: ";
			headersW += ConvertUTF8To16(contentType);
			headersW += L"\r\n";
		}

		if (!WinHttpSendRequest(hRequest, headersW.c_str(), headersW.length(),
		                        const_cast<char*>(data.c_str()), data.length(), data.length(), 0))
		{
			CryLogErrorAlways("HTTPClient: WinHttpSendRequest error code %lu", GetLastError());
			return;
		}
	}

	if (!WinHttpReceiveResponse(hRequest, NULL))
	{
		CryLogErrorAlways("HTTPClient: WinHttpReceiveResponse error code %lu", GetLastError());
		return;
	}

	// copy received data to the result string
	DWORD chunkLength = 0;
	do
	{
		char buffer[4096];
		if (!WinHttpReadData(hRequest, buffer, sizeof buffer, &chunkLength))
		{
			CryLogErrorAlways("HTTPClient: WinHttpReadData error code %lu", GetLastError());
			return;
		}

		result.append(buffer, chunkLength);
	}
	while (chunkLength > 0);

	DWORD statusCode;
	DWORD statusCodeLength = sizeof statusCode;
	if (!WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
	                         WINHTTP_HEADER_NAME_BY_INDEX, &statusCode, &statusCodeLength, WINHTTP_NO_HEADER_INDEX))
	{
		CryLogErrorAlways("HTTPClient: WinHttpQueryHeaders error code %lu", GetLastError());
		return;
	}

	status = statusCode;
}

void HTTPClient::GET(const char *url, TCallback pCallback, void *param)
{
	if (!url || !pCallback)
	{
		return;
	}

	HTTPClientRequestTask *pTask = new HTTPClientRequestTask();
	pTask->url = url;
	pTask->pCallback = pCallback;
	pTask->callbackParam = param;
	pTask->method = HTTPClientRequestTask::GET;

	Launcher::GetExecutor().addTask(pTask);
}

void HTTPClient::POST(const char *url, const char *data, const char *contentType, TCallback pCallback, void *param)
{
	if (!url || !pCallback)
	{
		return;
	}

	HTTPClientRequestTask *pTask = new HTTPClientRequestTask();
	pTask->url = url;
	pTask->pCallback = pCallback;
	pTask->callbackParam = param;
	pTask->method = HTTPClientRequestTask::POST;

	if (data)
	{
		pTask->data = data;
	}

	if (contentType)
	{
		pTask->contentType = contentType;
	}

	Launcher::GetExecutor().addTask(pTask);
}
