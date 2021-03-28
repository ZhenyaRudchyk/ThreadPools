#include <cstring>

#include <iostream>
#include <vector>

#include "DynamicPool.h"
#include "ScopedThread.h"
#include "Logger.h"
#include "TestTask.h"

using namespace thread_pool;
CLogger g_Logger;
int main(int argc, char const *argv[]) {
	thread_pool::CDynamicPool pool;

	std::vector<utils::CScopedThread> threads;
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
  
      pool.AddTaskWithoutResult([](){ 
          for (size_t i = 0; i < 10; ++i)
          {
          std::cout << "hello world!!!"<< std::endl;
      
          } });
};
  int newCounter = 0;
  while (newCounter != 5)
  {
	  threads.push_back(utils::CScopedThread(&CDynamicPool::AddTaskWithoutResult<std::function<void()>>, &pool, TaskToAdd));
	  newCounter++;
  }
  CTestTask test;
  auto CancelAction = test.GetCancellationFlag();
  pool.AddTaskWithoutResult([&test]() {test.Execute(); });

 *CancelAction = true;

   std::this_thread::sleep_for(std::chrono::seconds(5));
  return 0; 
}
