#pragma once


#include <atomic>
#include <thread>

#include "Util.h"
#include "ThreadSafeQueue.h"
#include "SkopedThread.h"

namespace tp_task
{
    class CDestroyWorker;
    class CMakeWorkerTask;
}

namespace tp
{
   class CDynamicPool 
   {
   public:
       CDynamicPool():
        m_iMaxWorkers(std::thread::hardware_concurrency()),
        m_iMinWorkers(1),
        m_WaitTime(1000),
        m_WorkersSize(0)
        {}

       CDynamicPool(const size_t minWorkers, const size_t maxWorkers, const std::chrono::milliseconds waitTime) :
           m_iMaxWorkers(maxWorkers),
           m_iMinWorkers(minWorkers),
           m_WaitTime(waitTime),
           m_WorkersSize(0)
       {}

        ~CDynamicPool();
        void Start();
	    void AddTaskWithoutResult(Task task);
   protected:
        void Run(CScopedThread* currentWorker);
        void CreateWorker();
        void DestroyWorker(std::thread::id threadID);
   private:
      DynamicThreadHashMapRAII m_Workers;
      int m_WorkersSize;
      std::mutex m_WorkersMutex;
      const size_t m_iMaxWorkers; // not const but should be change initialize only
      const size_t m_iMinWorkers; // not const but should be change in initialize only
      // return: number of threads increase to 1)Queue Size
      CThreadSafeQueue<tp::DynamicTask> m_TaskQueue;
      const std::chrono::milliseconds m_WaitTime;
   };
}

