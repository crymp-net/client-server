/**
 * @file
 * @brief Executor task interface.
 */

#pragma once

struct IExecutorTask
{
	virtual ~IExecutorTask() = default;

	// worker thread
	virtual void execute() = 0;

	// main thread
	virtual void callback() = 0;
};
