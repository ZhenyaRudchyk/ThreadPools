#include <iostream>
#include <thread>

#include "TestTask.h"

void CTestCancellableTask::Execute()
{
<<<<<<< Updated upstream
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
=======
	TASK_BEGIN
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "TestTask stage 1\n";
	INTERRUPTION_POINT
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "TestTask stage 2\n";
	INTERRUPTION_POINT
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout << "TestTask stage 3\n";
	TASK_END
>>>>>>> Stashed changes
}