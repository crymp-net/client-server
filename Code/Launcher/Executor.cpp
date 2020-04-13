/**
 * @file
 * @brief Implementation of executor.
 */

#include <new>

#include "Executor.h"
#include "IExecutorTask.h"

Executor::Executor() = default;

Executor::~Executor()
{
	m_isRunning = false;

	if (m_workerThread.joinable())
	{
		m_workerCV.notify_all();
		m_workerThread.join();
	}
}

void Executor::init()
{
	m_isRunning = true;

	if (!m_workerThread.joinable())
	{
		m_workerThread = std::thread(&Executor::workerLoop, this);
	}
}

void Executor::addTask(IExecutorTask *pTask)
{
	if (pTask)
	{
		m_workerQueue.push(pTask);
		m_workerCV.notify_one();
	}
}

void Executor::addTaskCallback(IExecutorTask *pTask)
{
	if (pTask)
	{
		m_completedQueue.push(pTask);
	}
}

void Executor::onUpdate()
{
	IExecutorTask *pTask = m_completedQueue.pop();
	if (pTask)
	{
		pTask->callback();
		delete pTask;
	}
}

void Executor::workerLoop()
{
	while (m_isRunning)
	{
		IExecutorTask *pTask = m_workerQueue.popWait(m_workerCV);
		if (pTask)
		{
			pTask->execute();
			m_completedQueue.push(pTask);
		}
	}
}
