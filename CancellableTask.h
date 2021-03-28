#pragma once

#include <atomic>
#include <memory>

namespace utils
{
	using ThreadSafeFlag = std::shared_ptr<std::atomic<bool>>;
	void CancelTask(ThreadSafeFlag& pTask);
	//Point were task can be interrupted. User should define this points in realization of Execute.
	#define CANNCELLATION_POINT if(IsCancelled()){ \
	m_TaskStatus = utils::TaskStatus::Finished; \
	m_TaskResult = utils::TaskResult::Cancelled; \
	return; }

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

	class CancellableTask
	{
	public:
		explicit CancellableTask():
			m_TaskStatus(TaskStatus::NotStarted),
			m_TaskResult(TaskResult::Unknown),
			m_CancellationFlag(std::make_shared<std::atomic<bool>>(false))
		{}
		CancellableTask(CancellableTask&& other) noexcept :
												m_CancellationFlag(std::move(other.m_CancellationFlag)),
												m_TaskStatus(other.m_TaskStatus.load()),
												m_TaskResult(other.m_TaskResult.load())
		{}

		CancellableTask& operator=(CancellableTask&& other) noexcept
		{
			if (this != &other)
			{
				m_CancellationFlag = std::move(other.m_CancellationFlag);
				m_TaskStatus = other.m_TaskStatus.load();
				m_TaskResult = other.m_TaskResult.load();
			}
			return *this;
		}

		virtual ~CancellableTask() {};
		virtual void Execute() = 0;
		ThreadSafeFlag GetCancellationHandler();
		TaskStatus GetTaskStatus() const;
		bool IsCancelled();
	protected: 
		ThreadSafeFlag m_CancellationFlag;
		std::atomic<TaskStatus> m_TaskStatus;
		std::atomic<TaskResult> m_TaskResult;
	};
}
