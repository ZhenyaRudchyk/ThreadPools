#include <cstring>

#include <iostream>


#include "ThreadPool.h"
#include "Logger.h"

CLogger g_Logger;
int main(int argc, char const *argv[]) {
	thread_pool::CDynamicPool pool;
try
{

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

    if(!pool.Initialize(3,20,std::chrono::milliseconds(50),[](uint64_t ui64QueueSize){ return  ui64QueueSize % 30;}))
    {
      break;
    }

    if(!pool.Start())
    {
      break;
    }
  } while (false);
  

  int i = 0;
  while (i< 1000)
  {
    pool.AddTaskWithoutResult([](int a){
	std::cout << "Hello World " + std::to_string(a) + '\n';
    }, i);
    i++;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    int b = 0;
  while (b< 100000)
  {
    pool.AddTaskWithoutResult([](int a){
    std::cout << "Hello World " + std::to_string(a) + '\n';
    }, b);
    b++;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(10000));
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
