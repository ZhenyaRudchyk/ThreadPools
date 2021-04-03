#pragma once

#include <atomic>
#include <queue>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <chrono>

#include "Util.h"
#include "Logger.h"

template<typename T>
 class CThreadSafeQueue
 {
 public:
#ifdef WAIT_MODE 
     CThreadSafeQueue() :
    m_PrimaryWaitTime(0),
    m_StopWaitAll(false)
    {}
   
    void WakeUpAllThreads();
    void SetPrimaryWaitTime(std::chrono::milliseconds PrimaryWaitTime); 
    void IncreaseWaitTime();

    thread_pool::WaitResult GetElement(T& element);
#else
    void GetElement(T& element);
#endif
    void AddElement(T& value );
    uint64_t Size() const;
	bool Empty();

 private:
#ifdef WAIT_MODE   
  std::condition_variable m_Notifier;
  std::atomic<std::chrono::milliseconds> m_WaitTime; // not synchronized
  std::chrono::milliseconds m_PrimaryWaitTime;
  std::atomic<bool> m_StopWaitAll;
#endif
  std::queue<T> m_Queue;
  mutable std::mutex m_Locker;

 };
////////////////////////////////////////////////////////////////////////////
template<typename T>
#ifdef WAIT_MODE 
typename thread_pool::WaitResult CThreadSafeQueue<T>::GetElement(T& element)
{
    if (m_StopWaitAll)
    {
        return thread_pool::WaitResult::WakeUpAll;
    }
#else
void CThreadSafeQueue<T>::GetElement(T& element)
{
#endif
    std::unique_lock<std::mutex> guard(m_Locker);
#ifdef WAIT_MODE 
    while (m_Queue.empty())
    {
        if(m_Notifier.wait_for(guard,m_WaitTime.load()) == std::cv_status::timeout)
        {
        #ifdef LOG
         g_Logger.WriteToFileWithNewLine("[GetElement] TimeOut = " + ThreadIDToString(std::this_thread::get_id()));
        #endif 
            return thread_pool::WaitResult::Timeout;
        }
        if(m_StopWaitAll)
        {
            return thread_pool::WaitResult::WakeUpAll;
        }
    }
#else
    if (m_Queue.empty())
    {
        return;
    }
#endif

    element = std::move(m_Queue.front());
    m_Queue.pop();
    guard.unlock();
#ifdef WAIT_MODE 
    if (m_WaitTime.load() != m_PrimaryWaitTime)
    {
        m_WaitTime.store(m_PrimaryWaitTime);
    }
#endif
    #ifdef LOG
    g_Logger.WriteToFileWithNewLine("[GetElement] QueueSize = " + std::to_string(m_Queue.size()));
    #endif
#ifdef WAIT_MODE 
    return thread_pool::WaitResult::GotElement;
#endif
}
////////////////////////////////////////////////////////////////////////////////
template<typename T>
void CThreadSafeQueue<T>::AddElement(T& value)
{
    {
        std::scoped_lock<std::mutex> guard(m_Locker);
        m_Queue.push(value);
        #ifdef LOG
            g_Logger.WriteToFileWithNewLine("[AddElement]  QueueSize = " + std::to_string(m_Queue.size()));
        #endif
    }
#ifdef WAIT_MODE 
    m_Notifier.notify_one();
#endif
}
////////////////////////////////////////////////////////////////////////////////
template<typename T>
uint64_t CThreadSafeQueue<T>::Size() const
{
    std::scoped_lock<std::mutex> guard(m_Locker);
    return static_cast<uint64_t>(m_Queue.size());
}
////////////////////////////////////////////////////////////////////////////////
#ifdef WAIT_MODE 
template<typename T>
void CThreadSafeQueue<T>::SetPrimaryWaitTime(std::chrono::milliseconds PrimaryWaitTime)
 {
     m_PrimaryWaitTime = PrimaryWaitTime;
     m_WaitTime.store(m_PrimaryWaitTime);
 }
#endif
////////////////////////////////////////////////////////////////////////////////
#ifdef WAIT_MODE 
template<typename T>
void CThreadSafeQueue<T>::WakeUpAllThreads()
 {
     m_StopWaitAll = true;
     m_Notifier.notify_all();
 }
#endif
////////////////////////////////////////////////////////////////////////////////
#ifdef WAIT_MODE 
 template<typename T>
 void CThreadSafeQueue<T>::IncreaseWaitTime()
 {
     std::chrono::milliseconds temp = m_WaitTime.load();
     temp *= 2;
     m_WaitTime.store(temp);
#ifdef LOG
     g_Logger.WriteToFileWithNewLine("[IncreaseWaitTime] m_WaitTime = " + std::to_string(m_WaitTime.load().count()));
#endif  
 }
#endif
 template<typename T>
 bool CThreadSafeQueue<T>::Empty()
 {
	 std::scoped_lock<std::mutex> guard(m_Locker);
	 return m_Queue.empty();
 }