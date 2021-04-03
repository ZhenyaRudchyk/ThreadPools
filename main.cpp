#include <cstring>

#include <iostream>
#include <vector>


#include "DynamicPool.h"
#include "Logger.h"
#include "TestTask.h"
#include "DobleQueueContainer.h"
#include "DynamicPool_V2.h"
#include "ThreadSafeQueue.h"

using namespace thread_pool;
//CLogger g_Logger;
int main(int argc, char const *argv[]) {
<<<<<<< Updated upstream
	thread_pool::CDynamicPool pool;

	std::vector<utils::CScopedThread> threads;
  do
  {
   /* if(!g_Logger.Initialize("File.txt"))
    {
      std::cout << "Initialize failed!!!";
      break;
    }

   if(!g_Logger.OpenFile())
   {
     break;
   }*/

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
          std::this_thread::sleep_for(std::chrono::milliseconds(200));
          } });
};
  int newCounter = 0;
  while (newCounter != 5)
  {
	  threads.push_back(utils::CScopedThread(&CDynamicPool::AddTaskWithoutResult<std::function<void()>>, &pool, TaskToAdd));
	  newCounter++;
  }

  CTestTask test;
  auto CancellationHandler = test.GetCancellationHandler();
  pool.AddTaskWithoutResult([&test]() {test.Execute(); });
  //utils::Task::CancelTask(CancellationHandler);
  *CancellationHandler = true;
 //std::this_thread::sleep_for(std::chrono::seconds(5));
=======
//	thread_pool::CDynamicPool pool;
//
//	std::vector<std::thread> threads;
//  do
//  {
//    if(!g_Logger.Initialize("File.txt"))
//    {
//      std::cout << "Initialize failed!!!";
//      break;
//    }
//
//   if(!g_Logger.OpenFile())
//   {
//     break;
//   }
//
//    if(!pool.Initialize(3,5,std::chrono::milliseconds(20),[](uint64_t ui64QueueSize){ return  ui64QueueSize ;}))
//    {
//      break;
//    }
//
//    if(!pool.Start())
//    {
//      break;
//    }
//  } while (false);
//  
//  auto TaskToAdd = [&pool]() {
//  
//      pool.AddTaskWithoutResult([](){ 
//          for (size_t i = 0; i < 100; ++i)
//          {
//			std::cout << "hello world!!!"<< std::endl;
//          } });
//};
//  int newCounter = 0;
//  while (newCounter != 5)
//  {
//	  threads.push_back(std::thread(&CDynamicPool::AddTaskWithoutResult<std::function<void()>>, &pool, TaskToAdd));
//	  newCounter++;
//  }
//
//
//  std::for_each(threads.begin(), threads.end(), [](std::vector<std::thread>::value_type& x) { x.join(); });
//
//  std::this_thread::sleep_for(std::chrono::seconds(1));

  //pool.AddTaskWithoutResult([]() {
  //    for (size_t i = 0; i < 100; ++i)
  //    {
  //        std::cout << "nEW hello world!!!" << std::endl;

  //    } });

  // CTestCancellableTask test;
////////////////////////////////////////////////V2/////////////////////////////////////////////////
	//std::vector<std::thread> producers;
	//std::vector<std::thread> consumers;
	//ThreadSafeQueueV2<int> test_q;
	//for (std::uint8_t i = 1; i < 10; ++i)
	//{
	//	producers.push_back(std::thread([&test_q]() {
	//		for (int i = 0; i < 100000; ++i)
	//		{
	//			test_q.push_back(i);
	//			//std::cout << "Size = " << test_q.Size() << std::endl;
	//		}
	//		}));
	//}


	//for (std::uint8_t i = 1; i < 10; ++i)
	//{
	//	consumers.push_back(std::thread([&test_q]() {
	//		for (int i = 0; i < 100000; ++i)
	//		{
	//			std::shared_ptr<int> pData;
	//			test_q.pop(pData);
	//			if (pData)
	//			{
	//				//std::cout << "Data =" << *pData << std::endl;
	//			}
	//			//std::cout << "Size = " << test_q.Size() << std::endl;
	//		}
	//		}));
	//}

	//for(auto& x : producers)
	//{
	//	x.join();
	//}

	//for (auto& x : consumers)
	//{
	//	x.join();
	//}
	////////////////////////////////////V1//////////////////////////////
	std::vector<std::thread> producers1;
	std::vector<std::thread> consumers1;
	CThreadSafeQueue<std::shared_ptr<int>> test_1;
	for (std::uint8_t i = 1; i < 10; ++i)
	{
		producers1.push_back(std::thread([&test_1]() {
			for (int i = 0; i < 100000; ++i)
			{
				auto NewData = std::make_shared<int>(i);
				test_1.AddElement(NewData);
				//std::cout << "Size = " << test_q.Size() << std::endl;
			}
			}));
	}


	for (std::uint8_t i = 1; i < 10; ++i)
	{
		consumers1.push_back(std::thread([&test_1]() {
			for (int i = 0; i < 100000; ++i)
			{
				std::shared_ptr<int> pData;
				test_1.GetElement(pData);
				if (pData)
				{
					//std::cout << "Data =" << *pData << std::endl;
				}
				//std::cout << "Size = " << test_q.Size() << std::endl;
			}
			}));
	}

	for (auto& x : producers1)
	{
		x.join();
	}

	for (auto& x : consumers1)
	{
		x.join();
	}



  //auto Task = test.GetCancellationHandler();
  //if (Task)
  //{
  //    pool.AddTaskWithoutResult([&test]() {test.Execute(); });
  //    //CANCEL_TASK(Task)
  //    std::this_thread::sleep_for(std::chrono::seconds(5));
  //}

>>>>>>> Stashed changes
  return 0; 
}
