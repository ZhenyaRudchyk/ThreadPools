#pragma once

#include <thread>

namespace tp
{
	class CScopedThread
	{
	public:
		using ThreadID = std::thread::id;
		CScopedThread() : m_WaitTime(0){};
		explicit CScopedThread(std::chrono::milliseconds waitTime) : m_WaitTime(waitTime) {};


		CScopedThread(CScopedThread&& other) noexcept:
			m_pRawThread(std::move(other.m_pRawThread)),
			m_WaitTime(other.m_WaitTime)
		{}

		CScopedThread& operator=(CScopedThread&& other)noexcept
		{
			if (&other != this)
			{
				m_pRawThread = std::move(other.m_pRawThread);
				m_WaitTime = other.m_WaitTime;
			}
			return *this;
		}

		void Construct(std::unique_ptr<std::thread> newThread)
		{
			m_pRawThread = std::move(newThread);
		}

		void Join()
		{
			if (m_pRawThread != nullptr)
			{
				if (m_pRawThread->joinable())
				{
					m_pRawThread->join();
				}
			}
		}

		~CScopedThread()
		{
			Join();
		}

		std::chrono::milliseconds GetWaitTime()
		{
			return m_WaitTime;
		}

		void SetWaitTime(std::chrono::milliseconds waitTime)
		{
			m_WaitTime = waitTime;
		}

		ThreadID GetID() const noexcept
		{
			if (m_pRawThread != nullptr)
			{
				return m_pRawThread->get_id();
			}
			else
			{
				_ASSERT(false);
				return {};
			}
		}

	private:
		std::unique_ptr<std::thread> m_pRawThread;
		std::chrono::milliseconds m_WaitTime;
	};
}


