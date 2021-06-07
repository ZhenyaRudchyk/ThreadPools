#pragma once

#include <atomic>
#include <queue>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <chrono>

#include "Util.h"


// Unbounded Blocking MPMC queue

template<typename T>
class CThreadSafeQueue
{
public:
    CThreadSafeQueue() :
        m_bStopWaitAll(false)
    {}
    void Shutdown();
    tp::WaitResult GetElementWithTimeout(std::unique_ptr<T>& element, std::chrono::milliseconds waitTime);
    void AddElement(std::unique_ptr<T> pValue);
    std::size_t Size() const;

private:
    std::condition_variable m_Notifier;
    bool m_bStopWaitAll;
    std::queue<std::unique_ptr<T>> m_Queue;
    mutable std::mutex m_Locker;
};

 ////////////////////////////////////////////////////////////////////////////
template<typename T>
tp::WaitResult CThreadSafeQueue<T>::GetElementWithTimeout(std::unique_ptr<T>& element, std::chrono::milliseconds waitTime)
{
    std::unique_lock<std::mutex> guard(m_Locker);
    while (m_Queue.empty())
    {
        if (m_bStopWaitAll)
        {
            return tp::WaitResult::WakeUpAll;
        }

        if(m_Notifier.wait_for(guard, waitTime) == std::cv_status::timeout)
        {
            return tp::WaitResult::Timeout;
        }
    }

    element = std::move(m_Queue.front());
    m_Queue.pop();

    return tp::WaitResult::GotElement;
}
////////////////////////////////////////////////////////////////////////////////
template<typename T>
void CThreadSafeQueue<T>::AddElement(std::unique_ptr<T> pValue)
{
    {
        std::scoped_lock<std::mutex> guard(m_Locker);
        m_Queue.push(std::move(pValue));
    }
    m_Notifier.notify_one();
}
////////////////////////////////////////////////////////////////////////////////
template<typename T>
size_t CThreadSafeQueue<T>::Size() const
{
    std::scoped_lock<std::mutex> guard(m_Locker);
    return m_Queue.size();
}
////////////////////////////////////////////////////////////////////////////////
template<typename T>
void CThreadSafeQueue<T>::Shutdown()
 {
     {
         std::scoped_lock<std::mutex> guard(m_Locker);
         m_bStopWaitAll = true;
     }
     m_Notifier.notify_all();
 }
////////////////////////////////////////////////////////////////////////////////
