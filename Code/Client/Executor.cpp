#include "Executor.h"

void ExecutorTaskQueue::push(std::unique_ptr<IExecutorTask> && task)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	m_queue.emplace_back(std::move(task));
}

std::unique_ptr<IExecutorTask> ExecutorTaskQueue::pop()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	std::unique_ptr<IExecutorTask> task;

	if (!m_queue.empty())
	{
		task = std::move(m_queue.front());
		m_queue.pop_front();
	}

	return task;
}

std::unique_ptr<IExecutorTask> ExecutorTaskQueue::popWait(std::condition_variable & cv)
{
	std::unique_lock<std::mutex> lock(m_mutex);

	std::unique_ptr<IExecutorTask> task;

	if (!m_queue.empty())
	{
		task = std::move(m_queue.front());
		m_queue.pop_front();
	}
	else
	{
		cv.wait(lock);
	}

	return task;
}

void Executor::workerLoop()
{
	while (m_isRunning)
	{
		std::unique_ptr<IExecutorTask> task = m_workerQueue.popWait(m_workerCV);
		if (task)
		{
			task->execute();

			m_completedQueue.push(std::move(task));
		}
	}
}

Executor::Executor()
{
	m_isRunning = true;

	if (!m_workerThread.joinable())
	{
		m_workerThread = std::thread(&Executor::workerLoop, this);
	}
}

Executor::~Executor()
{
	m_isRunning = false;

	if (m_workerThread.joinable())
	{
		m_workerCV.notify_all();
		m_workerThread.join();
	}
}

void Executor::addTask(std::unique_ptr<IExecutorTask> && task)
{
	m_workerQueue.push(std::move(task));
	m_workerCV.notify_one();
}

void Executor::addTaskCompleted(std::unique_ptr<IExecutorTask> && task)
{
	m_completedQueue.push(std::move(task));
}

void Executor::onUpdate()
{
	std::unique_ptr<IExecutorTask> task = m_completedQueue.pop();
	if (task)
	{
		task->callback();
	}
}
