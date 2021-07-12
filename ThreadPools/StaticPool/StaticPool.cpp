#include "StaticPool.h"

namespace tp 
{
	CStaticPool::CStaticPool(size_t nWorkersSize)
	{
		for (size_t i = 0; i < nWorkersSize; ++i)
		{
			CScopedThread temp;
			temp.Construct(std::make_unique<std::thread>(&CStaticPool::Run, this));
			m_Workers[temp.GetID()] = std::move(temp);
		}
	}
	CStaticPool::~CStaticPool()
	{
		m_TaskQueue.Shutdown();
	}

	void CStaticPool::Run()
	{
		while (true)
		{
			Task task;
			auto result = m_TaskQueue.GetElement(task);

			if (result == tp::WaitResult::GotElement)
			{
				if(task)
				{
					task();
				}
			} else if(result == tp::WaitResult::Stop)
			{
				break;
			}
		}
	}

	void CStaticPool::SubmitWithoutResult(Task& task)
	{
		if (task)
		{
			m_TaskQueue.AddElement(task);
		}
	}

}
