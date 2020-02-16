/**
 * @file
 * @brief Executor task interface.
 */

#pragma once

struct IExecutorTask
{
	virtual ~IExecutorTask()
	{
	}

	// worker thread
	virtual void execute() = 0;

	// main thread
	virtual void callback() = 0;
};
