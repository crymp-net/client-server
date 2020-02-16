/**
 * @file
 * @brief Implementation of executor.
 */

#include <new>
#include <deque>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Executor.h"
#include "IExecutorTask.h"

class LockGuard
{
	CRITICAL_SECTION *m_pCriticalSection;

public:
	LockGuard(CRITICAL_SECTION & criticalSection)
	: m_pCriticalSection(&criticalSection)
	{
		EnterCriticalSection(m_pCriticalSection);
	}

	~LockGuard()
	{
		LeaveCriticalSection(m_pCriticalSection);
	}
};

class TaskQueue
{
	std::deque<IExecutorTask*> m_queue;
	CRITICAL_SECTION m_criticalSection;

public:
	TaskQueue()
	: m_queue(),
	  m_criticalSection()
	{
		InitializeCriticalSection(&m_criticalSection);
	}

	~TaskQueue()
	{
		DeleteCriticalSection(&m_criticalSection);
	}

	void push(IExecutorTask *pTask)
	{
		LockGuard lock(m_criticalSection);

		m_queue.push_back(pTask);
	}

	IExecutorTask *pop()
	{
		LockGuard lock(m_criticalSection);

		if (m_queue.empty())
		{
			return NULL;
		}
		else
		{
			IExecutorTask *pTask = m_queue.front();
			m_queue.pop_front();
			return pTask;
		}
	}

	IExecutorTask *popWait(CONDITION_VARIABLE & conditionVariable)
	{
		LockGuard lock(m_criticalSection);

		if (m_queue.empty())
		{
			SleepConditionVariableCS(&conditionVariable, &m_criticalSection, INFINITE);
			return NULL;
		}
		else
		{
			IExecutorTask *pTask = m_queue.front();
			m_queue.pop_front();
			return pTask;
		}
	}
};

class Executor::Impl
{
	TaskQueue m_workerQueue;
	TaskQueue m_completedQueue;
	CONDITION_VARIABLE m_conditionVariable;
	HANDLE m_workerThread;
	bool m_isRunning;

	void WorkerLoop()
	{
		while (m_isRunning)
		{
			IExecutorTask *pTask = m_workerQueue.popWait(m_conditionVariable);
			if (pTask)
			{
				pTask->execute();
				m_completedQueue.push(pTask);
			}
		}
	}

	static DWORD WINAPI ThreadEntry(void *param)
	{
		Impl *self = static_cast<Impl*>(param);
		self->WorkerLoop();
		return 0;
	}

public:
	Impl()
	: m_workerQueue(),
	  m_completedQueue(),
	  m_conditionVariable(),
	  m_workerThread(),
	  m_isRunning()
	{
		InitializeConditionVariable(&m_conditionVariable);
	}

	~Impl()
	{
		m_isRunning = false;

		if (m_workerThread)
		{
			WakeAllConditionVariable(&m_conditionVariable);
			WaitForSingleObject(m_workerThread, INFINITE);

			CloseHandle(m_workerThread);
		}
	}

	bool init()
	{
		m_isRunning = true;

		if (!m_workerThread)
		{
			m_workerThread = CreateThread(NULL, 0, ThreadEntry, this, 0, NULL);
			if (!m_workerThread)
			{
				return false;
			}
		}

		return true;
	}

	void pushTask(IExecutorTask *pTask)
	{
		m_workerQueue.push(pTask);
		WakeConditionVariable(&m_conditionVariable);
	}

	void pushTaskCallback(IExecutorTask *pTask)
	{
		m_completedQueue.push(pTask);
	}

	void onUpdate()
	{
		IExecutorTask *pTask = m_completedQueue.pop();
		if (pTask)
		{
			pTask->callback();
			delete pTask;
		}
	}
};

Executor::Executor()
: m_impl(new Impl())
{
}

Executor::~Executor()
{
	delete m_impl;
}

bool Executor::init()
{
	return m_impl->init();
}

void Executor::addTask(IExecutorTask *pTask)
{
	if (pTask)
	{
		m_impl->pushTask(pTask);
	}
}

void Executor::addTaskCallback(IExecutorTask *pTask)
{
	if (pTask)
	{
		m_impl->pushTaskCallback(pTask);
	}
}

void Executor::onUpdate()
{
	m_impl->onUpdate();
}
