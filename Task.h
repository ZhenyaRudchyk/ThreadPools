#pragma once

#include <atomic>
#include <memory>


namespace utils
{
	using ThreadSafeFlag = std::shared_ptr<std::atomic<bool>>;
	//Point were task can be interrupted. User should define this points in realization of Execute.
	#define CANNCELLATION_POINT if(IsCancelled()){ return; }
	enum class TaskResult
	{
		Unknown,
		Success,
		Failed,
		Cancelled
	};

	enum class TaskStatus
	{
		NotStarted,
		Processing,
		Finished
	};

	class Task
	{
	public:
		Task():
			m_TaskStatus(TaskStatus::NotStarted),
			m_TaskResult(TaskResult::Unknown),
			m_CancellationFlag(std::make_shared<std::atomic<bool>>(false))
		{}
		virtual void Execute() = 0;
		void Cancel();
		ThreadSafeFlag GetCancellationFlag();
		TaskStatus GetTaskStatus() const;
		bool IsCancelled();
	protected: 
		ThreadSafeFlag m_CancellationFlag;
		std::atomic<TaskStatus> m_TaskStatus;
		std::atomic<TaskResult> m_TaskResult;
	};
}
