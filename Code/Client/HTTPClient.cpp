#include "Library/WinAPI.h"

#include "HTTPClient.h"
#include "Client.h"
#include "Executor.h"

struct HTTPClientTask : public IExecutorTask
{
	std::string method = "GET";
	std::string url;
	std::string data;
	std::map<std::string, std::string> headers;
	HTTPClient::Callback callback;
	HTTPClient::Result result;
	int timeout = HTTPClient::DEFAULT_TIMEOUT;
	bool cache = false;
	bool returnPath = false;

	// worker thread
	void Execute() override
	{
		// TODO: move this to Lua
		headers["X-Sfwcl-HWID"] = WinAPI::GetHWID("idsvc");
		headers["X-Sfwcl-Locale"] = WinAPI::GetLocale();
		headers["X-Sfwcl-Tz"] = std::to_string(WinAPI::GetTimeZoneBias());

		try
		{
			WinAPI::HTTPResponse response = WinAPI::HTTPRequest(method, url, data, headers, timeout, cache, returnPath);

			result.code = response.code;
			result.response = std::move(response.content);
		}
		catch (const Error & error)
		{
			result.error = error;
		}
	}

	// main thread
	void Callback() override
	{
		if (callback)
		{
			callback(result);
		}
	}
};

void HTTPClient::Request(
	const std::string_view & method,
	const std::string_view & url,
	const std::string_view & data,
	std::map<std::string, std::string> && headers,
	Callback callback,
	int timeout,
	bool cache,
	bool returnPath)
{
	std::unique_ptr<HTTPClientTask> task = std::make_unique<HTTPClientTask>();
	task->method = method;
	task->url = url;
	task->data = data;
	task->headers = std::move(headers);
	task->callback = std::move(callback);
	task->timeout = timeout;
	task->cache = cache;
	task->returnPath = returnPath;

	gClient->GetExecutor()->AddTask(std::move(task));
}

void HTTPClient::GET(
	const std::string_view & url,
	Callback callback,
	int timeout)
{
	std::unique_ptr<HTTPClientTask> task = std::make_unique<HTTPClientTask>();
	task->method = "GET";
	task->url = url;
	task->callback = std::move(callback);
	task->timeout = timeout;

	gClient->GetExecutor()->AddTask(std::move(task));
}

void HTTPClient::POST(
	const std::string_view & url,
	const std::string_view & data,
	const std::string_view & contentType,
	Callback callback,
	int timeout)
{
	std::unique_ptr<HTTPClientTask> task = std::make_unique<HTTPClientTask>();
	task->method = "POST";
	task->url = url;
	task->data = data;
	task->headers["Content-Type"] = contentType;
	task->callback = std::move(callback);
	task->timeout = timeout;

	gClient->GetExecutor()->AddTask(std::move(task));
}
