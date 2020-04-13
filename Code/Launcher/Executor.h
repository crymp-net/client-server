/**
 * @file
 * @brief Executor.
 */

#pragma once

#include <deque>
#include <mutex>
#include <condition_variable>
#include <thread>

struct IExecutorTask;

class ExecutorTaskQueue
{
	std::deque<IExecutorTask*> m_queue;
	std::mutex m_mutex;

public:
	ExecutorTaskQueue() = default;

	void push(IExecutorTask *pTask)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		m_queue.push_back(pTask);
	}

	IExecutorTask *pop()
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		IExecutorTask *pTask = nullptr;

		if (!m_queue.empty())
		{
			pTask = m_queue.front();
			m_queue.pop_front();
		}

		return pTask;
	}

	IExecutorTask *popWait(std::condition_variable & cv)
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		IExecutorTask *pTask = nullptr;

		if (!m_queue.empty())
		{
			pTask = m_queue.front();
			m_queue.pop_front();
		}
		else
		{
			cv.wait(lock);
		}

		return pTask;
	}
};

class Executor
{
	std::thread m_workerThread;
	std::condition_variable m_workerCV;
	ExecutorTaskQueue m_workerQueue;
	ExecutorTaskQueue m_completedQueue;
	bool m_isRunning;

	void workerLoop();

public:
	Executor();
	~Executor();

	void init();

	// thread-safe
	void addTask(IExecutorTask *pTask);
	void addTaskCallback(IExecutorTask *pTask);  // adds task directly to callback queue

	// main thread
	void onUpdate();
};
