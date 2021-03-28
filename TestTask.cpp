#include <iostream>
#include <thread>

#include "TestTask.h"

void CTestTask::Execute()
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "TestTask stage 1\n";
	CANNCELLATION_POINT
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "TestTask stage 2\n";
	CANNCELLATION_POINT
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "TestTask stage 3\n";
}