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

    if(!pool.Initialize(3,15,std::chrono::milliseconds(20),[](uint64_t ui64QueueSize){ return  ui64QueueSize % 30;}))
    {
      break;
    }

    if(!pool.Start())
    {
      break;
    }
  } while (false);
  
  auto TaskToAdd = [&pool]() {
   for (size_t i = 0; i < 10000; ++i)
   {
      pool.AddTaskWithoutResult([](){ std::this_thread::sleep_for(std::chrono::milliseconds(100));  });
   }
};
  int newCounter = 0;
  while (newCounter != 5)
  {
	  threads.push_back(std::thread(&CDynamicPool::AddTaskWithoutResult<std::function<void()>>, &pool, TaskToAdd));
	  newCounter++;
  }
  
  std::for_each(threads.begin(), threads.end(), [](std::thread& x) { x.join(); });

  //std::this_thread::sleep_for(std::chrono::seconds(50));
  return 0; 
}
//benchmark with time on linux
// 0.205