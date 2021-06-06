#pragma once


#include <atomic>
#include <thread>

#include "ITask.h"
#include "Util.h"
#include "ThreadSafeQueue.h" 


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
        m_fpIncreaseThreadsAlgorithm(nullptr),
        m_PoolStatus(PoolStatus::Stopped),
         m_iMaxWorkers(0),
         m_iMinWorkers(0),
        m_iInDesruction(0),
        m_WaitTime(0)
        {}
        ~CDynamicPool();

        bool Initialize(const size_t ui16MinWorkers,
                        const size_t ui16MaxWorkers,
                        std::chrono::milliseconds WaitTime,
                        IncreaseThreads fpIncreaseThreadsAlgorithm);
        bool Start();
	    void AddTaskWithoutResult(std::unique_ptr<tp_task::ITask> task);
   protected:
        void Run();   
        void StopAcceptingNewTasks();
   private:
      ThreadHashMap m_Workers;
      std::mutex m_WorkersMutex;
      size_t m_iMaxWorkers; // not const but should be change initialize only
      size_t m_iMinWorkers; // not const but should be change in initialize only
      // return: number of threads increase to 1)Queue Size
      IncreaseThreads m_fpIncreaseThreadsAlgorithm; // executes under multithreaded code. Take care about synchronization by yourself;
      std::atomic<PoolStatus> m_PoolStatus;
      size_t m_iInDesruction; // number of threads submited to destroy but are not destroyed yet
      CThreadSafeQueue<tp_task::ITask> m_TaskQueue;
      std::chrono::milliseconds m_WaitTime;
      friend class tp_task::CDestroyWorker;
      friend class tp_task::CMakeWorkerTask;
   };
}
