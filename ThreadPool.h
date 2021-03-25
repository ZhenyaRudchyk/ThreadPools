#pragma once

#include <atomic>
#include <cstdint>
#include <thread>
#include <unordered_map>
#include <future>
#include "Logger.h"
#include "ThreadSafeQueue.h"

namespace thread_pool
{

   enum class PoolStatus 
   {
      Stopped,
      RunningButDoNotConsumeNewTasks,
      Configured,
      Running
   };
   using ThreadID = std::thread::id;
   using Task = std::function<void()>;
   using ThreadMap = std::unordered_map<ThreadID,std::thread>;
   using IncreaseThreads = std::function<uint16_t(uint64_t)>;
   class CDynamicPool 
   {
   public:
      CDynamicPool():
      m_fpIncreaseThreadsAlgorithm(nullptr),
      m_PoolStatus(PoolStatus::Stopped),
      m_ui16MaxWorkers(0),
      m_ui16MinWorkers(0),
      WorkersCounter(0)
      {}
      ~CDynamicPool();

      bool Initialize(const uint16_t ui16MinWorkers,
                     const uint16_t ui16MaxWorkers,
                     std::chrono::milliseconds WaitTime,
                     IncreaseThreads fpIncreaseThreadsAlgorithm);
      bool Start();
      template <typename TCallable, typename ... TArgs>
      void AddTaskWithoutResult(TCallable &&func, TArgs &&... args);
       template <typename TCallable, typename ... TArgs>
   void AddTaskWithoutResult2(TCallable &&func, TArgs &&... args);
   // template <typename TCallable, typename ... TArgs>
      //auto AddTaskWithFutureResult(TCallable &&func, TArgs &&... args) -> std::future<typename std::result_of<TCallable(TArgs...)>::type>;
   protected:
         void Run();   
         void StopAcceptingNewTasks();
         bool CreateWorker();
         void DestroyWorker(const ThreadID & threadToDestory); 
   private:
      ThreadMap m_Workers;
      uint16_t WorkersCounter;
      std::mutex m_WorkersMutex;
      uint16_t m_ui16MaxWorkers;
      uint16_t m_ui16MinWorkers;
      // return: number of threads increase to 1)Queue Size
      IncreaseThreads m_fpIncreaseThreadsAlgorithm; // executes under multithreaded code. Take care about synchronization by yourself;
      std::atomic<PoolStatus> m_PoolStatus;
      CThreadSafeQueue<Task> m_TaskQueue;
   };
  

   CDynamicPool::~CDynamicPool()
   {
       #ifdef LOG
            g_Logger.WriteToFileWithNewLine("[CDynamicPool::~CDynamicPool] Workers = " + std::to_string(m_Workers.size()) +" "+ ThreadIDToString(std::this_thread::get_id()) + " Queue size =" + std::to_string(m_TaskQueue.Size()));
         #endif  
      StopAcceptingNewTasks();
      while (m_TaskQueue.Size() != 0)
      {
          std::this_thread::sleep_for(std::chrono::milliseconds(500));
      }
      m_PoolStatus = PoolStatus::Stopped;
      m_TaskQueue.WakeUpAllThreads();
      std::for_each(m_Workers.begin(), m_Workers.end(), [](ThreadMap::value_type& x){ (x.second).join(); });
   }
/////////////////////////////////////////////////////////////////////////////////////
   bool CDynamicPool::CreateWorker()
   {
      if (m_PoolStatus != PoolStatus::Stopped)
      {     
         if(WorkersCounter >= m_ui16MaxWorkers)
         {
            return false;
         } 
         std::thread tempThread (&CDynamicPool::Run, this);
         m_Workers[tempThread.get_id()] = std::move(tempThread);
         WorkersCounter++;
         #ifdef LOG
            g_Logger.WriteToFileWithNewLine("[CreateWorker] Workers = " + std::to_string(m_Workers.size()) +" "+ ThreadIDToString(std::this_thread::get_id()));
         #endif  
         return true;
      }

      return false;
   } 
/////////////////////////////////////////////////////////////////////////////////////
   void CDynamicPool::DestroyWorker(const ThreadID & IdThreadToDestory)
   {
      if (m_PoolStatus != PoolStatus::Stopped)
      {
        std::scoped_lock<std::mutex> guard(m_WorkersMutex);
         m_Workers[IdThreadToDestory].join();
         m_Workers.erase(IdThreadToDestory);
         #ifdef LOG
            g_Logger.WriteToFileWithNewLine("[DestroyWorker] Workers = " + std::to_string(m_Workers.size()));
         #endif  
      }
   }
/////////////////////////////////////////////////////////////////////////////////////
   void CDynamicPool::StopAcceptingNewTasks()
   {
      m_PoolStatus = PoolStatus::RunningButDoNotConsumeNewTasks;
   }
/////////////////////////////////////////////////////////////////////////////////////
   bool CDynamicPool::Start()
   {
      if(m_PoolStatus == PoolStatus::Configured)
      {
         for (uint16_t i = 0; i < m_ui16MinWorkers; ++i)
         {
            std::lock_guard<std::mutex> guard(m_WorkersMutex);
            if(!CreateWorker())
            {
               return false;
            }
         }
         return true;
      }
      return false;
   }
/////////////////////////////////////////////////////////////////////////////////////
   void CDynamicPool::Run()
   {
      if(m_PoolStatus != PoolStatus::Running)
      {
         m_PoolStatus == PoolStatus::Running;
      }

      while (m_PoolStatus != PoolStatus::Stopped)
      {
         Task taskToExecute = nullptr;
         auto getResult = m_TaskQueue.GetElement(taskToExecute);
         if(getResult == CThreadSafeQueue<Task>::WaitResult::Timeout)
         {
           std::lock_guard<std::mutex> guard(m_WorkersMutex);
            if(WorkersCounter > m_ui16MinWorkers)
            {
               #ifdef LOG
               g_Logger.WriteToFileWithNewLine("[Run] TimeOut = " + ThreadIDToString(std::this_thread::get_id())+ " Workers = " + std::to_string(m_Workers.size()));
               #endif
               // thread is not useful destroy it
               WorkersCounter--;
               AddTaskWithoutResult2([this](ThreadID temp1){DestroyWorker(temp1);}, std::this_thread::get_id());
               break;
            }
            else
            {
               m_TaskQueue.IncreaseWaitTime();
            }
         } 
         
         if(getResult == CThreadSafeQueue<Task>::WaitResult::GotElement)
         {
            if(taskToExecute)
            {
               taskToExecute();
            }
         }

      }
   }
/////////////////////////////////////////////////////////////////////////////////////
   // // template <typename TCallable, typename ... TArgs>
   // // auto CDynamicPool::AddTaskWithFutureResult(TCallable &&func, TArgs &&... args) -> std::future<typename std::result_of<TCallable(TArgs...)>::type>
   // // {
   // //    if(m_PoolStatus == PoolStatus::Running)
   // //    {
   // //    //get return type of the function
   // //      //typedef decltype(f(args...)) retType;
   // //       using PackedTask = std::packaged_task<typename std::result_of<TCallable(TArgs...)>::type()>;

   // //       auto task = std::make_shared<PackedTask>(std::bind(std::forward<TCallable>(func), std::forward<TArgs>(args)...));
   // //      //package the task
   // //       //std::packaged_task<retType()> task(std::move(std::bind(f, args...)));
   // //            //get the future from the task before the task is moved into the jobqueue
   // //      auto future = task->get_future();
   // //      // lock jobqueue mutex, add job to the job queue

   // //      //place the job into the queue
   // //      m_TaskQueue.AddElement([task](){ *task();});
        
   // //      {
   // //         std::scoped_lock<std::mutex> guard(m_WorkersMutex);
   // //          int64_t i64ThreadsToAdd = m_fpIncreaseThreadsAlgorithm(m_Workers.size(),m_TaskQueue.Size()) - m_Workers.size();
   // //          if(i64ThreadsToAdd > 0)
   // //          {
   // //             for(int64_t i = 0; i < i64ThreadsToAdd; ++i)
   // //             {
   // //                if(!CreateWorker())
   // //                {
   // //                   break;
   // //                }
   // //             }
   // //          }  
   // //      }
   // //      //return the future for the function so the user can get the return value
   // //      return future;
   // //    }
   // // }
/////////////////////////////////////////////////////////////////////////////////////
  template <typename TCallable, typename ... TArgs>
   void CDynamicPool::AddTaskWithoutResult(TCallable &&func, TArgs &&... args)
   {
      if(m_PoolStatus == PoolStatus::Running || m_PoolStatus == PoolStatus::Configured)
      {
      //get return type of the function
        //typedef decltype(f(args...)) retType;

         using PackedTask = std::packaged_task<typename std::result_of<TCallable(TArgs...)>::type()>;
         auto task = std::make_shared<PackedTask>(std::bind(std::forward<TCallable>(func), std::forward<TArgs>(args)...));
        //package the task
         //std::packaged_task<retType()> task(std::move(std::bind(f, args...)));
              //get the future from the task before the task is moved into the jobqueue
        // lock jobqueue mutex, add job to the job queue

        //place the job into the queue
        m_TaskQueue.AddElement([task](){ (*task)();});
        int64_t i64ThreadsToIncreaseNumber = m_fpIncreaseThreadsAlgorithm(m_TaskQueue.Size());
        {
           std::lock_guard<std::mutex> guard(m_WorkersMutex);
           std::uint16_t ui16ThreadsPerfectSize = i64ThreadsToIncreaseNumber - WorkersCounter;
               for(int64_t i = 0; i < ui16ThreadsPerfectSize; ++i)
               {
                  if(!CreateWorker())
                  {
                     break;
                  }
               }
              
        }
        //return the future for the function so the user can get the return value
      }
   }


   template <typename TCallable, typename ... TArgs>
   void CDynamicPool::AddTaskWithoutResult2(TCallable &&func, TArgs &&... args)
   {
      if(m_PoolStatus == PoolStatus::Running || m_PoolStatus == PoolStatus::Configured)
      {
      //get return type of the function
        //typedef decltype(f(args...)) retType;

         using PackedTask = std::packaged_task<typename std::result_of<TCallable(TArgs...)>::type()>;
         auto task = std::make_shared<PackedTask>(std::bind(std::forward<TCallable>(func), std::forward<TArgs>(args)...));
        //package the task
         //std::packaged_task<retType()> task(std::move(std::bind(f, args...)));
              //get the future from the task before the task is moved into the jobqueue
        // lock jobqueue mutex, add job to the job queue

        //place the job into the queue
        m_TaskQueue.AddElement([task](){ (*task)();});
        //return the future for the function so the user can get the return value
      }
   }
/////////////////////////////////////////////////////////////////////////////////////
   bool CDynamicPool::Initialize(const uint16_t ui16MinWorkers,
                   const uint16_t ui16MaxWorkers,
                   std::chrono::milliseconds WaitTime,
                   IncreaseThreads fpIncreaseThreadsAlgorithm)
   {
      m_ui16MinWorkers = ui16MinWorkers;
      m_ui16MaxWorkers = ui16MaxWorkers;
      if (m_ui16MaxWorkers <= m_ui16MinWorkers)
      {
         return false;
      }
      m_TaskQueue.SetPrimaryWaitTime(WaitTime);
      m_fpIncreaseThreadsAlgorithm = fpIncreaseThreadsAlgorithm;
      m_PoolStatus = PoolStatus::Configured;
      return true;
   }

}

