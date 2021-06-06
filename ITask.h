#pragma once
namespace tp_task
{
	class ITask
	{
	public:
		virtual void Execute() = 0;
		virtual ~ITask() = default;
	};
}
