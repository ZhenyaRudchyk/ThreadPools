#pragma once
#include <thread>
#include "ITask.h"
#include "Util.h"

namespace tp
{
	class CDynamicPool;
}

namespace tp_task
{

	class CDestroyWorker : public tp_task::ITask
	{
	public:
		CDestroyWorker(tp::CDynamicPool& threadPool, tp::ThreadID threadID):
			m_ThreadPool(threadPool),
			m_ThreadID(threadID)
		{}
		~CDestroyWorker() override {};
		void Execute() override;
	private:
		tp::CDynamicPool& m_ThreadPool;
		tp::ThreadID m_ThreadID;
	};

}
