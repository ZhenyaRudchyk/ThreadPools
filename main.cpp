#include <cstring>

#include <iostream>


#include "ThreadPool.h"
#include "Logger.h"

using namespace thread_pool;
CLogger g_Logger;
int main(int argc, char const *argv[]) {
	thread_pool::CDynamicPool pool;

	std::vector<std::thread> threads;
  do
  {
    if(!g_Logger.Initialize("File.txt"))
    {
      std::cout << "Initialize failed!!!";
      break;
    }

   if(!g_Logger.OpenFile())
   {
     break;
   }

    if(!pool.Initialize(3,20,std::chrono::milliseconds(400),[](uint64_t ui64QueueSize){ return  ui64QueueSize % 30;}))
    {
      break;
    }

    if(!pool.Start())
    {
      break;
    }
  } while (false);
  
  auto TaskToAdd = []() {
		  std::uint16_t tempCounter = 0;
		  while (tempCounter != 1000)
		  {
			 // std::cout << "Hello World!!! " << tempCounter << std::endl;
			  tempCounter++;
		  }
};
  int newCounter = 0;
  while (newCounter != 5)
  {
	  threads.push_back(std::thread(&CDynamicPool::AddTaskWithoutResult<std::function<void()>>, &pool, TaskToAdd));
	  newCounter++;
  }
  
  std::for_each(threads.begin(), threads.end(), [](std::thread& x) { x.join(); });
  return 0; 
}
