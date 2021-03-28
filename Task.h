#pragma once

#include <atomic>
#include <memory>


namespace utils
{
	//#define TASK_BEGIN m_TaskStatus = utils::TaskStatus::Processing; 

#define CANNCELLATION_POINT if (IsCancelled()){ return; }
			//m_TaskStatus = utils::TaskStatus::Finished; \
			//m_TaskResult = utils::TaskResult::Cancelled; 


//#define TASK_END \
			m_TaskStatus = utils::TaskStatus::Finished; \
			m_TaskResult = utils::TaskResult::Success; 


	using ThreadSafeFlag = std::shared_ptr<std::atomic<bool>>;

	//Point were task can be interrupted. User should define this points in realization of Execute.

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
		ThreadSafeFlag GetCancellationHandler();
		TaskStatus GetTaskStatus() const;
		static void CancelTask(ThreadSafeFlag& task);
		bool IsCancelled();
	protected: 
		ThreadSafeFlag m_CancellationFlag;
		std::atomic<TaskStatus> m_TaskStatus;
		std::atomic<TaskResult> m_TaskResult;
	};
}
