#include <cstring>
#include <cmath> 
#include <iostream>
#include <vector>
#include "StaticPool.h"

using namespace tp;
int main(int argc, char const* argv[]) {

	//  try
	//  {

	//  CDynamicPool pool;

	//do
	//{


	//  if(!pool.Initialize(7,30,std::chrono::milliseconds(100),[](size_t queueSize){ return log(queueSize);}))
	//  {
	//    break;
	//  }

	//  if(!pool.Start())
	//  {
	//    break;
	//  }

	//} while (false);
	//


	//  for (size_t i = 0; i < 10; i++)
	//  {
	//      pool.AddTaskWithoutResult([]() {std::cout << 1; });
	//  }
	//    //  std::this_thread::sleep_for(std::chrono::seconds(2));

	//  }

	//  catch (const std::exception& a)
	//  {
	//      std::cout << a.what();
	//  }
	//  
	std::mutex mutex1;
	CStaticPool a(5);
	for (size_t i = 1; i <= 1000; i++)
	{
		Task task([i, &mutex1]() {
			std::unique_lock<std::mutex>(mutex1);
			std::cout << i << std::endl; });
		a.SubmitWithoutResult(task);
	}

	std::this_thread::sleep_for(std::chrono::seconds(15));


	return 0;
}
