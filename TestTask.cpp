#include <iostream>
#include <thread>

#include "TestTask.h"

void CTestTask::Execute()
{
	//TASK_BEGIN
	std::this_thread::sleep_for(std::chrono::seconds(2));
	std::cout << "TestTask stage 1\n";
	CANNCELLATION_POINT
	std::this_thread::sleep_for(std::chrono::seconds(2));
	std::cout << "TestTask stage 2\n";
	CANNCELLATION_POINT
	std::this_thread::sleep_for(std::chrono::seconds(2));
	std::cout << "TestTask stage 3\n";
//	TASK_END
}