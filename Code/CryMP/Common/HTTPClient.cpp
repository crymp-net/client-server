#include "CryCommon/CrySystem/ISystem.h"
#include "Library/Util.h"
#include "Library/WinAPI.h"

#include "HTTPClient.h"
#include "Executor.h"

struct HTTPClientTask : public IExecutorTask
{
	HTTPClientRequest request;
	HTTPClientResult result;

	// worker thread
	void Execute() override
	{
		CryLog("%s %s", request.method.c_str(), request.url.c_str());

		try
		{
			result.code = WinAPI::HTTPRequest(
				request.method,
				request.url,
				request.data,
				request.headers,
				[this](uint64_t contentLength, const WinAPI::HTTPRequestReader & reader)
				{
					// content length is zero if not provided by the server
					result.response.reserve(contentLength);

					while (true)
					{
						char chunk[8192];
						const size_t chunkLength = reader(chunk, sizeof chunk);

						if (chunkLength == 0)
							break;

						result.response.append(chunk, chunkLength);
					}
				}
			);
		}
		catch (const std::exception& ex)
		{
			result.error = ex.what();
		}
	}

	// main thread
	void Callback() override
	{
		if (request.callback)
		{
			request.callback(result);
		}
	}
};

HTTPClient::HTTPClient(Executor& executor) : m_executor(&executor)
{
}

HTTPClient::~HTTPClient()
{
}

void HTTPClient::Request(HTTPClientRequest&& request)
{
	std::unique_ptr<HTTPClientTask> task = std::make_unique<HTTPClientTask>();
	task->request = std::move(request);

	m_executor->AddTask(std::move(task));
}
