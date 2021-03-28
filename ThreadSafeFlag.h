#pragma once
#include <memory>
#include <mutex>

class CThreadSafeFlag
{
public:
	CThreadSafeFlag():
		m_pFlag(std::make_shared<bool>(false))
	{
		
	}
	bool Is_Set()
	{
		std::scoped_lock<std::mutex> guard(m_Locker);
		return *m_pFlag;
	}
	void Set()
	{
		std::scoped_lock<std::mutex> guard(m_Locker);
		*m_pFlag = true;
	}

private:
	std::mutex m_Locker;
	std::shared_ptr<bool> m_pFlag;
};