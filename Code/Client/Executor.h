#pragma once

#include <deque>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>

struct IExecutorTask
{
	virtual ~IExecutorTask() = default;

	// worker thread
	virtual void execute() = 0;

	// main thread
	virtual void callback() = 0;
};

class ExecutorTaskQueue
{
	std::deque<std::unique_ptr<IExecutorTask>> m_queue;
	std::mutex m_mutex;

public:
	ExecutorTaskQueue() = default;

	void push(std::unique_ptr<IExecutorTask> && task);

	std::unique_ptr<IExecutorTask> pop();
	std::unique_ptr<IExecutorTask> popWait(std::condition_variable & cv);
};

class Executor
{
	std::thread m_workerThread;
	std::condition_variable m_workerCV;
	ExecutorTaskQueue m_workerQueue;
	ExecutorTaskQueue m_completedQueue;
	bool m_isRunning = false;

	void workerLoop();

public:
	Executor();
	~Executor();

	// thread-safe
	void addTask(std::unique_ptr<IExecutorTask> && task);
	void addTaskCompleted(std::unique_ptr<IExecutorTask> && task);

	// main thread
	void onUpdate();
};
