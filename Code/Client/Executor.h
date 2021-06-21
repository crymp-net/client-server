#pragma once

#include <deque>
#include <thread>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <functional>

struct IExecutorTask
{
	virtual ~IExecutorTask() = default;

	// worker thread
	virtual void Execute() = 0;

	// main thread
	virtual void Callback() = 0;
};

class ExecutorTaskQueue
{
	std::deque<std::unique_ptr<IExecutorTask>> m_queue;
	std::mutex m_mutex;

public:
	ExecutorTaskQueue() = default;

	void Push(std::unique_ptr<IExecutorTask> && task);

	std::unique_ptr<IExecutorTask> Pop();
	std::unique_ptr<IExecutorTask> PopWait(std::condition_variable & cv);
};

class Executor
{
	std::thread m_workerThread;
	std::condition_variable m_workerCV;
	ExecutorTaskQueue m_workerQueue;
	ExecutorTaskQueue m_completedQueue;
	bool m_isRunning = false;

	void WorkerLoop();

public:
	Executor();
	~Executor();

	// main thread
	void OnUpdate();

	// thread-safe
	void AddTask(std::unique_ptr<IExecutorTask> && task);
	void AddTaskCompleted(std::unique_ptr<IExecutorTask> && task);

	// alternative to IExecutorTask
	using Lambda = std::function<void()>;

	// thread-safe
	void AddLambda(Lambda onExecute, Lambda onCallback);
	void AddLambdaCompleted(Lambda onCallback);
};
