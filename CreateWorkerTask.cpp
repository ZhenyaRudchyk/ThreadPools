#include <iostream>

#include "CreateWorkerTask.h"
#include "DynamicPool.h"

namespace tp_task
{

	void CMakeWorkerTask::Execute()
	{

        for (size_t i = 0; i < m_ullThreadsToCreate; ++i)
        {
            std::scoped_lock<std::mutex> guard(m_ThreadPool.m_WorkersMutex);
            if (m_ThreadPool.m_Workers.size() < m_ThreadPool.m_iMaxWorkers)
            {
                std::thread tempThread(&tp::CDynamicPool::Run, &m_ThreadPool);

                m_ThreadPool.m_Workers[tempThread.get_id()] = std::move(tempThread);
            }
            else
            {
                return;
            }
        }
	}

}