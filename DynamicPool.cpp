#include <iostream>
#include <memory>
#include <chrono>
#include "CreateWorkerTask.h"
#include "DynamicPool.h"
#include "DestroyThreadTask.h"

namespace tp
{

    /////////////////////////////////////////////////////////////////////////////////////  
    CDynamicPool::~CDynamicPool()
    {
        StopAcceptingNewTasks();
        m_TaskQueue.Shutdown();
        std::for_each(m_Workers.begin(), m_Workers.end(), [](ThreadHashMap::value_type& x) {
            (x.second).join(); });
    }
    /////////////////////////////////////////////////////////////////////////////////////
    void CDynamicPool::StopAcceptingNewTasks()
    {
        m_PoolStatus = PoolStatus::Stopped;
    }
    /////////////////////////////////////////////////////////////////////////////////////
    void CDynamicPool::Run()
    {
        if (m_PoolStatus != PoolStatus::Running)
            m_PoolStatus = PoolStatus::Running;

        while (true)
        {
            std::unique_ptr<tp_task::ITask> task;

            tp::WaitResult result = m_TaskQueue.GetElementWithTimeout(task, m_WaitTime);

            if (result == WaitResult::Timeout)
            {
                std::unique_lock<std::mutex> guard(m_WorkersMutex);
                if ((m_Workers.size() - m_iInDesruction) >  m_iMinWorkers)
                {
                    ++m_iInDesruction;
                    guard.unlock();
                    m_TaskQueue.AddElement(std::make_unique<tp_task::CDestroyWorker>(*this, std::this_thread::get_id()));
                    break;
                }
                else // if min threads no sense king of busy wait with timeout condvar, just wait with big timeout for new tasks
                {
                    guard.unlock();
                    result = m_TaskQueue.GetElementWithTimeout(task, std::chrono::milliseconds::max());
                }

            }
            else if (result == WaitResult::GotElement)
            {
                if (task)
                {
                    task->Execute(); // need to take care about exceptions
                }
            }
            else if (result == WaitResult::WakeUpAll)
            {
                break;
            }
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////
    void CDynamicPool::AddTaskWithoutResult(std::unique_ptr<tp_task::ITask> pTask)
    {
        if ((m_PoolStatus == PoolStatus::Running) && pTask)
        {
            m_TaskQueue.AddElement(std::move(pTask));
            if (SYNCHRONIZED_INVOCABLE(m_WorkersMutex, m_Workers.size()) !=  m_iMaxWorkers)
            {
                m_TaskQueue.AddElement(std::make_unique<tp_task::CMakeWorkerTask>(*this, m_fpIncreaseThreadsAlgorithm(m_TaskQueue.Size())));
            }
        }
    }
    /////////////////////////////////////////////////////////////////////////////////////
    bool CDynamicPool::Initialize(const size_t ui16MinWorkers,
        const size_t ui16MaxWorkers,
        std::chrono::milliseconds waitTime,
        IncreaseThreads fpIncreaseThreadsAlgorithm)
    {
         m_iMinWorkers = ui16MinWorkers;
         m_iMaxWorkers = ui16MaxWorkers;
        if ( m_iMaxWorkers <=  m_iMinWorkers ||  m_iMaxWorkers < 0 ||  m_iMinWorkers < 0)
        {
            return false;
        }
        m_fpIncreaseThreadsAlgorithm = fpIncreaseThreadsAlgorithm;
        m_WaitTime = waitTime;
        m_PoolStatus = PoolStatus::Configured;
        return true;
    }
    /////////////////////////////////////////////////////////////////////////////////////
    bool CDynamicPool::Start()
    {
        auto pCreateWorker = std::make_unique<tp_task::CMakeWorkerTask>(*this,  m_iMinWorkers);
        pCreateWorker->Execute();
        return true;
    }
}