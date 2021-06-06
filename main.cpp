#include <cstring>

#include <iostream>
#include <vector>
#include "DummyTask.h"
#include "DynamicPool.h"

using namespace tp;
int main(int argc, char const *argv[]) {
	
    try
    {

    CDynamicPool pool;

  do
  {


    if(!pool.Initialize(7,30,std::chrono::milliseconds(100),[](size_t ui64QueueSize){ return  ui64QueueSize % 90;}))
    {
      break;
    }

    if(!pool.Start())
    {
      break;
    }

  } while (false);
  


    for (size_t i = 0; i < 10000; i++)
    {
        auto dummyTask = std::make_unique<DummyTask>();
        pool.AddTaskWithoutResult(std::move(dummyTask));
    }
    std::this_thread::sleep_for(std::chrono::seconds(15));

    std::this_thread::sleep_for(std::chrono::seconds(10));
    for (size_t i = 0; i < 10000; i++)
    {
        auto dummyTask = std::make_unique<DummyTask>();
        pool.AddTaskWithoutResult(std::move(dummyTask));
    }

    }

    catch (const std::exception& a)
    {
        std::cout << a.what();
    }
    
  return 0; 
}
