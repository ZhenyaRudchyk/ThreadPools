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
////////////////////////////////////////////////////////////////////////////////
    void Shutdown()
    {
        {
            std::scoped_lock<std::mutex> guard(m_Locker);
            m_bStopWaitAll = true;
        }
        m_Notifier.notify_all();
    }
////////////////////////////////////////////////////////////////////////////////
    tp::WaitResult GetElementWithTimeout(T& element, const std::chrono::milliseconds waitTime)
    {
        std::unique_lock<std::mutex> guard(m_Locker);

        while (m_Queue.empty() && !m_bStopWaitAll)
        {
            if (m_Notifier.wait_for(guard, waitTime) == std::cv_status::timeout)
            {
                return tp::WaitResult::Timeout;
            }
        }

        if (m_bStopWaitAll)
        {
            return tp::WaitResult::Stop;
        }
        else
        {
            element = std::move(m_Queue.front());
            m_Queue.pop();
            return tp::WaitResult::GotElement;
        }
    }
////////////////////////////////////////////////////////////////////////////////
    tp::WaitResult GetElement(T& element)
    {
        std::unique_lock<std::mutex> guard(m_Locker);
        while (m_Queue.empty() && !m_bStopWaitAll)
        {
            m_Notifier.wait(guard);
        }
        if (m_bStopWaitAll)
        {
            return tp::WaitResult::Stop;
        }
        else
        {
            element = std::move(m_Queue.front());
            m_Queue.pop();
            return tp::WaitResult::GotElement;
        }
    }
////////////////////////////////////////////////////////////////////////////////
    void AddElement(T& element)
    {
        {
            std::scoped_lock<std::mutex> guard(m_Locker);
            if (!m_bStopWaitAll)
            {
                m_Queue.push(element);
            }
            else
            {
                return;
            }
        }
        m_Notifier.notify_one();
    }
////////////////////////////////////////////////////////////////////////////////
    void AddElement(T&& element)
    {
        {
            std::scoped_lock<std::mutex> guard(m_Locker);
            if (!m_bStopWaitAll)
            {
                m_Queue.push(std::move(element));
            }
            else
            {
                return;
            }
        }
        m_Notifier.notify_one();
    }
////////////////////////////////////////////////////////////////////////////////
    std::size_t Size() const
    {
        std::scoped_lock<std::mutex> guard(m_Locker);
        return m_Queue.size();
    }

private:
    std::condition_variable m_Notifier;
    bool m_bStopWaitAll;
    std::queue<T> m_Queue;
    mutable std::mutex m_Locker;
};


