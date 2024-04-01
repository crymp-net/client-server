#include <thread>

#include "CryLog.h"
#include "CryPak.h"
#include "StreamEngine.h"

StreamEngine StreamEngine::s_globalInstance;

////////////////////////////////////////////////////////////////////////////////
// IReadStream
////////////////////////////////////////////////////////////////////////////////

void StreamEngine::Job::AddRef()
{
	this->refCount.fetch_add(1, std::memory_order_relaxed);
}

void StreamEngine::Job::Release()
{
	if (this->refCount.fetch_sub(1, std::memory_order_relaxed) <= 1)
	{
		delete this;
	}
}

bool StreamEngine::Job::IsError()
{
	return this->isError.load(std::memory_order_relaxed);
}

bool StreamEngine::Job::IsFinished()
{
	return this->isFinished.load(std::memory_order_relaxed);
}

unsigned int StreamEngine::Job::GetBytesRead(bool wait)
{
	if (wait)
	{
		this->Wait();
	}

	return this->bytesRead.load(std::memory_order_relaxed);
}

const void* StreamEngine::Job::GetBuffer()
{
	return this->userBuffer;
}

void StreamEngine::Job::Abort()
{
	this->isAborted.store(true, std::memory_order_relaxed);
	this->isFinished.store(true, std::memory_order_relaxed);
}

void StreamEngine::Job::RaisePriority(unsigned int priority)
{
	CryLogAlways("%s(%u): NOT IMPLEMENTED!", __FUNCTION__, priority);
}

std::uintptr_t StreamEngine::Job::GetUserData()
{
	return this->userData;
}

void StreamEngine::Job::Wait()
{
	std::unique_lock lock(this->mutex);
	this->cv.wait(lock, [this]() { return this->isFinished.load(std::memory_order_relaxed); });
}

////////////////////////////////////////////////////////////////////////////////

void StreamEngine::Init()
{
	// TODO: no detach once the global instance is gone
	std::thread([this]() { this->WorkerThreadLoop(); }).detach();
}

StreamEngine::StreamEngine()
{
}

StreamEngine::~StreamEngine()
{
}

////////////////////////////////////////////////////////////////////////////////
// IStreamEngine
////////////////////////////////////////////////////////////////////////////////

IReadStreamPtr StreamEngine::StartRead(const char* source, const char* filename, IStreamCallback* callback,
	StreamReadParams* params)
{
	_smart_ptr<Job> job(new Job);
	job->callback = callback;

	if (source)
	{
		job->source = source;
	}

	if (filename)
	{
		job->filename = filename;
	}

	if (params)
	{
		job->userData = params->dwUserData;
		job->userBuffer = params->pBuffer;
		job->requestedOffset = params->nOffset;
		job->requestedSize = params->nSize;
		job->flags = params->nFlags;
	}

	CryLogAlways("%s(\"%s\", \"%s\", size=%u, offset=%u, flags=0x%x)", __FUNCTION__, source, filename,
		job->requestedSize, job->requestedOffset, job->flags);

	m_submittedJobs.PushAndNotify(job);

	return IReadStreamPtr(job.get());
}

unsigned int StreamEngine::GetFileSize(const char* filename, unsigned int cryPakFlags)
{
	auto handle = CryPak::GetInstance().FOpen(filename, "rb");
	if (!handle)
	{
		return 0;
	}

	const unsigned int size = static_cast<unsigned int>(CryPak::GetInstance().FGetSize(handle));

	CryPak::GetInstance().FClose(handle);

	return size;
}

void StreamEngine::Update(unsigned int flags)
{
	_smart_ptr<Job> job;

	while (m_completedJobs.TryPop(job))
	{
		if (job)
		{
			this->FinishJob(*job);
		}
	}
}

unsigned int StreamEngine::Wait(unsigned int milliseconds, unsigned int flags)
{
	CryLogAlways("%s(%u, 0x%x): NOT IMPLEMENTED!", __FUNCTION__, milliseconds, flags);
	return 0;
}

void StreamEngine::GetMemoryStatistics(ICrySizer* sizer)
{
}

void StreamEngine::SuspendCallbackTimeQuota()
{
	CryLogAlways("%s: NOT IMPLEMENTED!", __FUNCTION__);
}

void StreamEngine::ResumeCallbackTimeQuota()
{
	CryLogAlways("%s: NOT IMPLEMENTED!", __FUNCTION__);
}

////////////////////////////////////////////////////////////////////////////////

void StreamEngine::WorkerThreadLoop()
{
	_smart_ptr<Job> job;

	while (true)
	{
		m_submittedJobs.BlockingPop(job);

		if (!job)
		{
			break;
		}

		this->ExecuteJob(*job);

		job->isFinished.store(true, std::memory_order_relaxed);
		job->cv.notify_all();

		m_completedJobs.Push(job);
	}
}

void StreamEngine::ExecuteJob(Job& job)
{
	auto handle = CryPak::GetInstance().FOpen(job.filename.c_str(), "rb");
	if (!handle)
	{
		job.isError.store(true, std::memory_order_relaxed);
		return;
	}

	if (job.requestedOffset > 0)
	{
		CryPak::GetInstance().FSeek(handle, job.requestedOffset, SEEK_SET);
	}

	auto size = job.requestedSize;
	if (!size)
	{
		size = static_cast<unsigned int>(CryPak::GetInstance().FGetSize(handle));

		if (job.requestedOffset < size)
		{
			size -= job.requestedOffset;
		}
		else
		{
			size = 0;
		}
	}

	if (!job.userBuffer)
	{
		job.buffer = std::make_unique<unsigned char[]>(size);
		job.userBuffer = job.buffer.get();
	}

	auto bytesRead = CryPak::GetInstance().FReadRaw(job.userBuffer, 1, size, handle);

	job.bytesRead.store(static_cast<unsigned int>(bytesRead), std::memory_order_relaxed);

	CryPak::GetInstance().FClose(handle);
}

void StreamEngine::FinishJob(Job& job)
{
	if (!job.callback)
	{
		return;
	}

	unsigned int error = 0;

	if (job.isAborted.load(std::memory_order_relaxed))
	{
		error = ERROR_USER_ABORT;
	}
	else if (job.isError.load(std::memory_order_relaxed))
	{
		error = ERROR_CANT_OPEN_FILE;
	}

	job.callback->StreamOnComplete(&job, error);
}
