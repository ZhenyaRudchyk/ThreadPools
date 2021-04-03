#pragma once

#include "Task.h"

class CTestCancellableTask : public utils::CancellableTask
{
public:
	CTestCancellableTask():utils::CancellableTask()
	{}
	~CTestCancellableTask() override
	{}
	void Execute();
};
