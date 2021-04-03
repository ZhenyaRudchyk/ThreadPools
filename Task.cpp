#include "Task.h"

<<<<<<< Updated upstream

namespace utils {
	ThreadSafeFlag Task::GetCancellationHandler()
=======
namespace utils 
{
/////////////////////////////////////////////////////////////////////////////////////////
	ThreadSafeFlag CancellableTask::GetCancellationHandler()
>>>>>>> Stashed changes
	{
		m_CancellationFlag = std::make_shared<std::atomic<bool>>();
		return m_CancellationFlag;
	}
/////////////////////////////////////////////////////////////////////////////////////////
<<<<<<< Updated upstream
	TaskStatus Task::GetTaskStatus() const
=======
	TaskStatus CancellableTask::GetTaskStatus() const noexcept
>>>>>>> Stashed changes
	{
		return m_TaskStatus;
	}
/////////////////////////////////////////////////////////////////////////////////////////
<<<<<<< Updated upstream
	void Task::CancelTask(ThreadSafeFlag& task)
	{
		*task = true;
	}
/////////////////////////////////////////////////////////////////////////////////////////
	bool Task::IsCancelled()
=======
	bool CancellableTask::IsCancelled() const noexcept
>>>>>>> Stashed changes
	{
		std::cout << "IsCancelled " << *m_CancellationFlag << std::endl;
		return *m_CancellationFlag;
	}
/////////////////////////////////////////////////////////////////////////////////////////
}
