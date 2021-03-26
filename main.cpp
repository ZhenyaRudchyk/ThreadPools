#include <cstring>

#include <iostream>


#include "ThreadPool.h"
#include "Logger.h"

using namespace thread_pool;
CLogger g_Logger;
int main(int argc, char const *argv[]) {
	thread_pool::CDynamicPool pool;
try
{
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

    if(!pool.Initialize(3,50,std::chrono::milliseconds(400),[](uint64_t ui64QueueSize){ return  ui64QueueSize % 30;}))
    {
      break;
    }

    if(!pool.Start())
    {
      break;
    }
  } while (false);
  
  auto TaskToAdd = []() {
		  std::uint8_t tempCounter = 0;
		  while (tempCounter != 10000 )
		  {
			  tempCounter++;
		  }};
  int newCounter = 0;
  while (newCounter != 10)
  {
	  threads.push_back(std::thread(&CDynamicPool::AddTaskWithoutResult<std::function<void()>>, &pool, []() {
		  std::uint8_t tempCounter = 0;
		  while (tempCounter != 10000)
		  {
			  tempCounter++;
		  }}));
	  newCounter++;
  }
}
catch(const std::exception& e)
{
  std::cerr << e.what() << '\n';
}

  
  // CLogger log;
  // do
  // {
  //   if(!log.Initialize("File.txt"))
  //   {
  //     std::cout << "Initialize failed!!!";
  //     break;
  //   }

//   if(!log.OpenFile())
//   {
//     break;
//   }
    
  //   if(!log.WriteToFileWithNewLine("I am new line!!!"))
  //   {
  //     break;
  //   }
  // } while (false);
  
 
  return 0; 
}
