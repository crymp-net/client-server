/**
 * @file
 * @brief Executor.
 */

#pragma once

struct IExecutorTask;

class Executor
{
	class Impl;
	Impl *m_impl;  // std::unique_ptr is C++11

public:
	Executor();
	~Executor();

	bool init();

	// thread-safe
	void addTask(IExecutorTask *pTask);
	void addTaskCallback(IExecutorTask *pTask);  // adds task directly to callback queue

	// main thread
	void onUpdate();
};
