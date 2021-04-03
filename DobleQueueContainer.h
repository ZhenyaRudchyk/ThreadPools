#pragma once
#include <array>
#include <vector>
#include <atomic>
#include <memory>
#include <mutex>

#include "Util.h"


template<typename T>
class ThreadSafeQueueV2
{
public:
	//struct Node;
struct Node
 {
	std::shared_ptr<T> m_pData;
	std::unique_ptr<Node> m_pNext;
 }; 

	ThreadSafeQueueV2():
#ifdef WAIT_MODE 
		m_PrimaryWaitTime(0),
		m_StopWaitAll(false),
#endif
		m_ui64ElementsCounter(0),
		m_pHead(new Node), // fake empty  node
		m_pTail(m_pHead.get())
	{}
#ifdef WAIT_MODE 
	thread_pool::WaitResult pop(std::unique_ptr<T> & element)
#else
	void pop(std::shared_ptr<T>& element)
#endif
	{
#ifdef WAIT_MODE 
		if (m_StopWaitAll)
		{
			return thread_pool::WaitResult::WakeUpAll;
		}
		std::unique_lock<std::mutex> head_guard(m_HeadLocker);

		while(m_pHead.get() == get_tail())
		{


			if (m_Notifier.wait_for(head_guard, m_WaitTime.load()) == std::cv_status::timeout)
			{
				element = nullptr;
				return thread_pool::WaitResult::Timeout;
			}

			if (m_StopWaitAll)
			{
				return thread_pool::WaitResult::WakeUpAll;
			}
		}
#else
		std::unique_lock<std::mutex> head_guard(m_HeadLocker);
		if (m_pHead.get() == get_tail())
		{
			element = nullptr;
			return;
		}
#endif

		element = std::move(m_pHead->m_pData);
		auto pOldHead = std::move(m_pHead);
		m_pHead = std::move(pOldHead->m_pNext);
		head_guard.unlock();
		--m_ui64ElementsCounter;
#ifdef WAIT_MODE 
		return thread_pool::WaitResult::GotElement;
#endif
	}

	void push_back(T&& element)
	{
		auto dataForCurrentFake = std::make_shared<T>(std::move(element));
		auto newFake = std::make_unique<Node>();
		std::unique_lock<std::mutex> tail_guard(m_TailLocker);
		m_pTail->m_pData = std::move(dataForCurrentFake);
		m_pTail->m_pNext = std::move(newFake);
		m_pTail = m_pTail->m_pNext.get();
		tail_guard.unlock();
		++m_ui64ElementsCounter;
#ifdef WAIT_MODE 
		m_Notifier.notify_one(); ;
#endif
	}

	void push_back(T& element)
	{
		auto dataForCurrentFake = std::make_shared<T>(element);
		auto newFake = std::make_unique<Node>();
		std::unique_lock<std::mutex> tail_guard(m_TailLocker);
		m_pTail->m_pData = std::move(dataForCurrentFake);
		m_pTail->m_pNext = std::move(newFake);
		m_pTail = m_pTail->m_pNext.get();
		tail_guard.unlock();
		++m_ui64ElementsCounter;
#ifdef WAIT_MODE 
		m_Notifier.notify_one(); ;
#endif
	}


	std::uint64_t Size()
	{
		return m_ui64ElementsCounter.load();
	}

	bool Empty()
	{
		return m_ui64ElementsCounter == 0;
	}
#ifdef WAIT_MODE 
	void SetPrimaryWaitTime(std::chrono::milliseconds PrimaryWaitTime)
	{
		m_PrimaryWaitTime = PrimaryWaitTime;
		m_WaitTime = m_PrimaryWaitTime;
	}

	void WakeUpAllThreads()
	{
		m_StopWaitAll.store(true);
		m_Notifier.notify_all();
	}
	
	void IncreaseWaitTime()
	{
		std::chrono::milliseconds temp = m_WaitTime.load();
		temp *= 2;
		m_WaitTime.store(temp);
	}
#endif
private:

	Node* get_tail()
	{
		std::scoped_lock<std::mutex> tail_guard(m_TailLocker);
		return m_pTail;
	}


	std::unique_ptr<Node> m_pHead;
    Node* m_pTail;
	std::mutex m_HeadLocker;
	std::mutex m_TailLocker;
#ifdef WAIT_MODE 
	std::condition_variable m_Notifier;
	std::atomic<bool> m_StopWaitAll;
	std::atomic<std::chrono::milliseconds> m_WaitTime;
	std::chrono::milliseconds m_PrimaryWaitTime;
#endif
	std::atomic<std::uint64_t> m_ui64ElementsCounter;
};
