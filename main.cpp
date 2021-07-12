#include <cstring>
#include <cmath> 
#include <iostream>
#include <vector>
#include "DynamicPool.h"

using namespace tp;
int main(int argc, char const *argv[]) {
	
    try
    {

        CDynamicPool pool;

          pool.Start();
  

      std::mutex mutex1;
      for (size_t i = 1; i <= 100; i++)
      {
          Task task([i, &mutex1]() {
              std::unique_lock<std::mutex>(mutex1);
              std::cout << i << std::endl; });
          pool.AddTaskWithoutResult(task);
      }

      //std::this_thread::sleep_for(std::chrono::seconds(10));


          for (size_t i = 1; i <= 100; i++)
          {
              Task task([i, &mutex1]() {
                  std::unique_lock<std::mutex>(mutex1);
                  std::cout << i << std::endl; });
              pool.AddTaskWithoutResult(task);
          }
          std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    catch (const std::exception& a)
    {
        std::cout << a.what();
    }
    


	


  return 0; 
}
