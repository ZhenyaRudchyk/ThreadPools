#pragma once

#include <mutex>

#include "./../../Util.h"
#include "./../../ThreadSafeQueue.h"

namespace tp
{
	class CStaticPool
	{
	public:
		CStaticPool(size_t nWorkersSize);
		~CStaticPool();
		void SubmitWithoutResult(Task& task);
	private:
		void Run();
		ThreadHashMapRAII m_Workers;
		CThreadSafeQueue<Task> m_TaskQueue;
	};
}
