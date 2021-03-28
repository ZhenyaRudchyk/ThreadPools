#pragma once 

#include "Util.h"


namespace thread_pool
{
	class CStaticPool
	{
		public:
			bool Initialize(const uint16_t ui16Workers);
			bool Start();
			template <typename TCallable, typename ... TArgs>
			void AddTaskWithoutResult(TCallable &&func, TArgs &&... args);
			// template <typename TCallable, typename ... TArgs>
			   //auto AddTaskWithFutureResult(TCallable &&func, TArgs &&... args) -> std::future<typename std::result_of<TCallable(TArgs...)>::type>;
	protected:
		void Run();
		void StopAcceptingNewTasks();
		bool CreateWorker();
		//void DestroyWorker(const ThreadID threadToDestory);
		void WaitUntilQueueEmpty();
	private:
	};
}