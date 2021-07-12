#include <iostream>
#include <memory>
#include <chrono>
#include "DynamicPool.h"

namespace tp
{
	CDynamicPool::~CDynamicPool()
	{
		m_TaskQueue.Shutdown();
	}

	void CDynamicPool::Start()
	{
		std::scoped_lock<std::mutex> guard(m_WorkersMutex);
		for (size_t i = 0; i < m_iMinWorkers; ++i)
		{
			std::unique_ptr<CScopedThread> temp = std::make_unique<CScopedThread>(m_WaitTime);
			temp->Construct(std::make_unique<std::thread>(&CDynamicPool::Run, this, temp.get()));
			m_Workers[temp->GetID()] = std::move(temp);
			m_WorkersSize++;
		}
	}

	void CDynamicPool::CreateWorker()
	{
	
		std::scoped_lock<std::mutex> guard(m_WorkersMutex);
		if (m_WorkersSize < m_iMaxWorkers)
		{
			std::unique_ptr<CScopedThread> temp = std::make_unique<CScopedThread>(m_WaitTime);
			temp->Construct(std::make_unique<std::thread>(&CDynamicPool::Run, this, temp.get()));
			m_Workers[temp->GetID()] = std::move(temp);
			m_WorkersSize++;
		}
	}

	void CDynamicPool::DestroyWorker(std::thread::id threadID)
	{
			std::scoped_lock<std::mutex> guard(m_WorkersMutex);
			auto it = m_Workers.find(threadID);
			if (it != m_Workers.end())
			{
				if (m_WorkersSize <= m_iMinWorkers)
				{
					auto waitTime = it->second->GetWaitTime();
					it->second->Join();
					m_Workers.erase(it);
					std::unique_ptr<CScopedThread> temp = std::make_unique<CScopedThread>(waitTime*2);
					temp->Construct(std::make_unique<std::thread>(&CDynamicPool::Run, this, temp.get()));
					m_Workers[temp->GetID()] = std::move(temp);
					
				}
				else
				{
					it->second->Join();
					m_Workers.erase(it);
					--m_WorkersSize;
				}
			}
			else
			{
				_ASSERT(false);
			}
	}

	void CDynamicPool::AddTaskWithoutResult(Task task)
	{
		if (task)
		{
			m_TaskQueue.AddElement(DynamicTask{std::move(task), false});
			CreateWorker();
		}
	}

	void CDynamicPool::Run(CScopedThread * currentWorker)
	{
		while (true)
		{
			DynamicTask currentTast;
			auto getTaskResult = m_TaskQueue.GetElementWithTimeout(currentTast, currentWorker->GetWaitTime());
			
			if (getTaskResult == WaitResult::Timeout)
			{
				m_TaskQueue.AddElement({ [this, threadID = std::this_thread::get_id()] () {this->DestroyWorker(threadID); }, true });
				break;
			}
			else if (getTaskResult == WaitResult::GotElement)
			{
				if (!currentTast.second && currentWorker->GetWaitTime() != m_WaitTime)
				{
					currentWorker->SetWaitTime(m_WaitTime);
				}

				if (currentTast.first)
				{
					currentTast.first();
				}
			}
			else if (getTaskResult == WaitResult::Stop)
			{
				break;
			}
		}
	}
}
