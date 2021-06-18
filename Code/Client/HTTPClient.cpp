#include <windows.h>
#include <winhttp.h>

#include "Library/WinAPI.h"

#include "HTTPClient.h"
#include "Client.h"
#include "Executor.h"
#include <sstream>
#include <fstream>

#define HTTP_CLIENT_USER_AGENT L"CryMP-Client"

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

struct HTTPClientTask : public IExecutorTask
{

	std::string m_method = "GET";
	std::string m_url;
	std::string m_data;
	std::map<std::string, std::string> m_headers;
	HTTPClient::Callback m_callback;
	int m_timeout = 0;
	std::string m_outputPath;
	bool m_cache = false;
	bool m_returnPath = false;

	int m_status = 0;
	std::string m_reply;
	SystemError m_error;

	// worker thread
	void execute() override;

	// main thread
	void callback() override
	{
		if (m_callback)
		{
			m_callback(m_status, m_reply, m_error);
		}
	}
};

void HTTPClient::Request(
	const std::string_view& method,
	const std::string_view& url,
	const std::map<std::string, std::string>& headers,
	const std::string_view& body,
	Callback callback,
	int timeout,
	bool cache,
	bool returnPath
) {
	std::unique_ptr<HTTPClientTask> task = std::make_unique<HTTPClientTask>();
	task->m_method = method;
	task->m_url = url;
	task->m_data = body;
	task->m_headers = headers;
	task->m_callback = std::move(callback);
	task->m_timeout = timeout;
	task->m_cache = cache;
	task->m_returnPath = returnPath;

	gClient->getExecutor()->addTask(std::move(task));
}

void HTTPClient::GET(const std::string_view & url, Callback callback, int timeout)
{
	std::unique_ptr<HTTPClientTask> task = std::make_unique<HTTPClientTask>();
	task->m_method = "GET";
	task->m_url = url;
	task->m_callback = std::move(callback);
	task->m_timeout = timeout;

	gClient->getExecutor()->addTask(std::move(task));
}

void HTTPClient::POST(const std::string_view & url, const std::string_view & data,
                      const std::string_view & contentType, Callback callback, int timeout)
{
	std::unique_ptr<HTTPClientTask> task = std::make_unique<HTTPClientTask>();
	task->m_method = "POST";
	task->m_url = url;
	task->m_data = data;
	task->m_headers["Content-type"] = contentType;
	task->m_callback = std::move(callback);
	task->m_timeout = timeout;

	gClient->getExecutor()->addTask(std::move(task));
}

void HTTPClientTask::execute()
{
	if (m_cache) {
		m_outputPath = WinAPI::GetCachePath(m_url);
		if (m_outputPath.length() > 0) {
			std::ifstream f(m_outputPath, std::ios::binary);
			if (f) {
				m_status = 200;
				if (m_returnPath) {
					m_reply = m_outputPath;
				} else {
					std::stringstream ss;
					ss << f.rdbuf();
					m_reply = ss.str();
				}
				f.close();
				return;
			}
		}
	}
	const std::wstring url = WinAPI::ConvertUTF8To16(m_url);

	URL_COMPONENTS urlComponents = {};
	urlComponents.dwStructSize = sizeof urlComponents;
	urlComponents.dwSchemeLength = -1;
	urlComponents.dwHostNameLength = -1;
	urlComponents.dwUrlPathLength = -1;
	urlComponents.dwExtraInfoLength = -1;

	if (!WinHttpCrackUrl(url.c_str(), url.length(), 0, &urlComponents))
	{
		m_error = SystemError("WinHttpCrackUrl");
		return;
	}

	HTTPHandleGuard hSession = WinHttpOpen(HTTP_CLIENT_USER_AGENT,
	                                       WINHTTP_ACCESS_TYPE_NO_PROXY,
	                                       WINHTTP_NO_PROXY_NAME,
	                                       WINHTTP_NO_PROXY_BYPASS, 0);
	if (!hSession)
	{
		m_error = SystemError("WinHttpOpen");
		return;
	}

	if (!WinHttpSetTimeouts(hSession, m_timeout, m_timeout, m_timeout, m_timeout))
	{
		m_error = SystemError("WinHttpSetTimeouts");
		return;
	}

	std::wstring serverName(urlComponents.lpszHostName, urlComponents.dwHostNameLength);

	HTTPHandleGuard hConnect = WinHttpConnect(hSession, serverName.c_str(), urlComponents.nPort, 0);
	if (!hConnect)
	{
		m_error = SystemError("WinHttpConnect");
		return;
	}

	DWORD requestFlags = WINHTTP_FLAG_REFRESH;

	if (urlComponents.nScheme == INTERNET_SCHEME_HTTPS)
	{
		requestFlags |= WINHTTP_FLAG_SECURE;
	}

	// URL components are not null-terminated, but whole URL is, so URL path contains both path and parameters
	HTTPHandleGuard hRequest = WinHttpOpenRequest(hConnect,
	                                              WinAPI::ConvertUTF8To16(m_method).c_str(),
	                                              urlComponents.lpszUrlPath, nullptr,
	                                              WINHTTP_NO_REFERER,
	                                              WINHTTP_DEFAULT_ACCEPT_TYPES, requestFlags);
	if (!hRequest)
	{
		m_error = SystemError("WinHttpOpenRequest");
		return;
	}

	m_headers["X-Sfwcl-HWID"] = WinAPI::GetHWID("idsvc");
	m_headers["X-Sfwcl-Locale"] = WinAPI::GetLocale();
	m_headers["X-Sfwcl-Tz"] = std::to_string(WinAPI::GetTimeZoneBias());
	std::wstring headers;

	for (auto& [key, value] : m_headers) {
		headers += WinAPI::ConvertUTF8To16(key) + L": " + WinAPI::ConvertUTF8To16(value) + L"\r\n";
	}

	if (!WinHttpSendRequest(hRequest, headers.c_str(), headers.length(),
	                        const_cast<char*>(m_data.c_str()), m_data.length(), m_data.length(), 0))
	{
		m_error = SystemError("WinHttpSendRequest");
		return;
	}

	if (!WinHttpReceiveResponse(hRequest, nullptr))
	{
		m_error = SystemError("WinHttpReceiveResponse");
		return;
	}

	DWORD chunkLength = 0;
	do
	{
		char buffer[4096];
		if (!WinHttpReadData(hRequest, buffer, sizeof buffer, &chunkLength))
		{
			m_error = SystemError("WinHttpReadData");
			return;
		}

		m_reply.append(buffer, chunkLength);
	}
	while (chunkLength > 0);

	DWORD code;
	DWORD codeLength = sizeof code;
	if (!WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
	                         WINHTTP_HEADER_NAME_BY_INDEX, &code, &codeLength, WINHTTP_NO_HEADER_INDEX))
	{
		m_error = SystemError("WinHttpQueryHeaders");
		return;
	}

	m_status = code;

	if (m_cache && m_outputPath.length() > 0 && m_status >= 200 && m_status < 305) {
		std::ofstream f(m_outputPath, std::ios::binary);
		if (!f) {
			m_error = SystemError("Cannot open cache for write");
			return;
		}
		f << m_reply;
		f.close();
		if (m_returnPath) {
			m_reply = m_outputPath;
;		}
	}
}
