#include "ITask.h"

namespace tp_task
{
	bool operator < (const ITask& left, const ITask& right)
	{
		return left.m_Priority < right.m_Priority;
	}

	bool operator > (const ITask& left, const ITask& right)
	{
		return left.m_Priority > right.m_Priority;
	}
}