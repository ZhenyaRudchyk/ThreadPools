#pragma once

#include <atomic>
#include <iostream>
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
<<<<<<< Updated upstream

	//Point were task can be interrupted. User should define this points in realization of Execute.

	enum class TaskResult
	{
		Unknown,
		Success,
		Failed,
		Cancelled
	};
=======
	// add at the beginning of task
	#define TASK_BEGIN m_TaskStatus = utils::TaskStatus::Processing;
	//Point were task can be interrupted. User should define this points in realization of Execute.
	#define INTERRUPTION_POINT if(IsCancelled()){  m_TaskStatus = utils::TaskStatus::Cancelled; return; }
	// add at the end of task
	#define TASK_END m_TaskStatus = utils::TaskStatus::Success;
	// cancel task
	#define CANCEL_TASK(task) {*task = true; }
>>>>>>> Stashed changes

	enum class TaskStatus
	{
		NotStarted,
		Processing,
		Success,
		Failed,
		Cancelled
	};

	class CancellableTask
	{
	public:
		CancellableTask() noexcept:
			m_TaskStatus(TaskStatus::NotStarted)
		{}

		CancellableTask(CancellableTask&& other) noexcept:
			m_TaskStatus(other.m_TaskStatus.load()),
			m_CancellationFlag(std::move(other.m_CancellationFlag)),
			m_sError(std::move(other.m_sError))
		{}

		CancellableTask& operator= (CancellableTask&& other) noexcept
		{
			if (this != &other)
			{
				m_TaskStatus = other.m_TaskStatus.load();
				m_CancellationFlag = std::move(other.m_CancellationFlag);
				m_sError = std::move(other.m_sError);
			}
			return *this;
		}

		virtual ~CancellableTask()
		{}

		virtual void Execute() = 0;
		ThreadSafeFlag GetCancellationHandler();
<<<<<<< Updated upstream
		TaskStatus GetTaskStatus() const;
		static void CancelTask(ThreadSafeFlag& task);
		bool IsCancelled();
=======
		TaskStatus GetTaskStatus() const noexcept;
		bool IsCancelled() const noexcept;
>>>>>>> Stashed changes
	protected: 
		ThreadSafeFlag m_CancellationFlag;
		std::atomic<TaskStatus> m_TaskStatus;
		// check if status Failed 
		std::string m_sError;
	};
}
