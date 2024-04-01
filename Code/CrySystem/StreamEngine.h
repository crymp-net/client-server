#pragma once

#include <atomic>
#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <string>

#include "CryCommon/CrySystem/IStreamEngine.h"

class StreamEngine final : public IStreamEngine
{
	template<class T>
	class CompletionQueue
	{
	protected:
		std::mutex mutex;
		std::deque<T> queue;

	public:
		void Push(const T& element)
		{
			std::lock_guard lock(this->mutex);
			this->queue.push_back(element);
		}

		bool TryPop(T& element)
		{
			std::lock_guard lock(this->mutex);
			if (this->queue.empty())
			{
				return false;
			}
			else
			{
				element = this->queue.front();
				this->queue.pop_front();
				return true;
			}
		}
	};

	template<class T>
	class SubmissionQueue : private CompletionQueue<T>
	{
		std::condition_variable cv;

	public:
		void PushAndNotify(const T& element)
		{
			this->Push(element);
			this->cv.notify_one();
		}

		void BlockingPop(T& element)
		{
			std::unique_lock lock(this->mutex);
			this->cv.wait(lock, [this]() { return !this->queue.empty(); });
			element = this->queue.front();
			this->queue.pop_front();
		}
	};

	struct Job final : public IReadStream
	{
		std::atomic<int> refCount = 0;
		std::atomic<bool> isError = false;
		std::atomic<bool> isAborted = false;
		std::atomic<bool> isFinished = false;
		std::atomic<unsigned int> bytesRead = 0;
		unsigned int flags = 0;
		unsigned int requestedSize = 0;
		unsigned int requestedOffset = 0;
		void* userBuffer = nullptr;
		IStreamCallback* callback = nullptr;
		std::uintptr_t userData = 0;
		std::string source;
		std::string filename;
		std::unique_ptr<unsigned char[]> buffer;
		std::mutex mutex;
		std::condition_variable cv;

		////////////////////////////////////////////////////////////////////////////////
		// IReadStream
		////////////////////////////////////////////////////////////////////////////////

		void AddRef() override;
		void Release() override;

		bool IsError() override;
		bool IsFinished() override;

		unsigned int GetBytesRead(bool wait = false) override;
		const void* GetBuffer() override;

		void Abort() override;

		void RaisePriority(unsigned int priority) override;

		std::uintptr_t GetUserData() override;

		void Wait() override;

		////////////////////////////////////////////////////////////////////////////////
	};

	SubmissionQueue<_smart_ptr<Job>> m_submittedJobs;
	CompletionQueue<_smart_ptr<Job>> m_completedJobs;

	static StreamEngine s_globalInstance;

public:
	// to be removed once we have our own CrySystem
	static StreamEngine& GetInstance()
	{
		return s_globalInstance;
	}

	// and this too
	void Init();

	StreamEngine();
	~StreamEngine();

	////////////////////////////////////////////////////////////////////////////////
	// IStreamEngine
	////////////////////////////////////////////////////////////////////////////////

	IReadStreamPtr StartRead(const char* source, const char* filename, IStreamCallback* callback = nullptr,
		StreamReadParams* params = nullptr) override;

	unsigned int GetFileSize(const char* filename, unsigned int cryPakFlags = 0) override;

	void Update(unsigned int flags = 0) override;

	unsigned int Wait(unsigned int milliseconds, unsigned int flags = 0) override;

	void GetMemoryStatistics(ICrySizer* sizer) override;

	void SuspendCallbackTimeQuota() override;
	void ResumeCallbackTimeQuota() override;

	////////////////////////////////////////////////////////////////////////////////

private:
	void WorkerThreadLoop();
	void ExecuteJob(Job& job);
	void FinishJob(Job& job);
};
