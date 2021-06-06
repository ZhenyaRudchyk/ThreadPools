#pragma once
#include <iostream>
#include <thread>
#include <chrono>
#include "ITask.h"

class DummyTask : public tp_task::ITask
{
public:

	void Execute() override
	{
		std::cout << 1;
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	~DummyTask() override
	{}
};