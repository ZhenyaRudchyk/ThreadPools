#include <iostream>
#include "DestroyThreadTask.h"
#include "Util.h"
#include "DynamicPool.h"

namespace tp_task
{
    void CDestroyWorker::Execute()
    {
        std::scoped_lock<std::mutex> guard(m_ThreadPool.m_WorkersMutex);
        if (m_ThreadPool.m_Workers.size() >= m_ThreadPool.m_iMinWorkers)
        {
            m_ThreadPool.m_Workers[m_ThreadID].join();
            m_ThreadPool.m_Workers.erase(m_ThreadID);
            --m_ThreadPool.m_iInDesruction;
        }
    }
}