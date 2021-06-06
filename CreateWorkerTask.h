#pragma once

#include "ITask.h"
#include "Util.h"


namespace tp
{
	class CDynamicPool;
}

namespace tp_task
{
	class CMakeWorkerTask : public tp_task::ITask
	{
	public:
		CMakeWorkerTask(tp::CDynamicPool& threadPool, size_t ullThreadsToCreate):
			m_ThreadPool(threadPool),
			m_ullThreadsToCreate(ullThreadsToCreate)
		{}

		~CMakeWorkerTask() override
		{};
		void Execute() override;

	private:
		tp::CDynamicPool& m_ThreadPool;
		size_t m_ullThreadsToCreate;
	};
}