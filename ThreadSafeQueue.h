#pragma once
#include <atomic>
#include <queue>
#include <functional>
#include <condition_variable>
#include <mutex>
#include <chrono>
#include "Logger.h"

template<typename T>
 class CThreadSafeQueue
 {
 public:
  enum class WaitResult
  {
      Unknown,
      GotElement,
      Timeout,
      WakeUpAll
  };
  
    CThreadSafeQueue():
    m_WaitTime(0),
    m_PrimaryWaitTime(0),
    m_StopWaitAll(false)
    {}
    
    void WakeUpAllThreads();
    void SetPrimaryWaitTime(std::chrono::milliseconds PrimaryWaitTime); 
    void IncreaseWaitTime();
    WaitResult GetElement(T& element);
    void AddElement(const T& value );
    uint64_t Size() const;

 private:
  std::condition_variable m_Notifier;
  std::queue<T> m_Queue;
  mutable std::mutex m_Locker;
  std::chrono::milliseconds m_WaitTime;
  std::chrono::milliseconds m_PrimaryWaitTime;
  std::atomic<bool> m_StopWaitAll;
 };
////////////////////////////////////////////////////////////////////////////
template<typename T>
typename CThreadSafeQueue<T>::WaitResult CThreadSafeQueue<T>::GetElement(T& element)
{
    if(m_StopWaitAll)
    {
        return WaitResult::WakeUpAll;
    }

    std::unique_lock<std::mutex> guard(m_Locker);
    while (m_Queue.empty())
    {
        #ifdef LOG
         g_Logger.WriteToFileWithNewLine("[GetElement] QueueSize = " + std::to_string(m_Queue.size()));
        #endif  
        if(m_Notifier.wait_for(guard,m_WaitTime) == std::cv_status::timeout)
        {
        #ifdef LOG
         g_Logger.WriteToFileWithNewLine("[GetElement] TimeOut = " + ThreadIDToString(std::this_thread::get_id()));
        #endif 
            return WaitResult::Timeout;
        }
        if(m_StopWaitAll)
        {
            return WaitResult::WakeUpAll;
        }
    }

    element = std::move(m_Queue.front());
    m_Queue.pop();
    m_WaitTime = m_PrimaryWaitTime;
    return WaitResult::GotElement;
}
////////////////////////////////////////////////////////////////////////////////
template<typename T>
void CThreadSafeQueue<T>::AddElement(const T& value)
{
    {
        std::scoped_lock<std::mutex> guard(m_Locker);
        m_Queue.push(value);
        #ifdef LOG
            g_Logger.WriteToFileWithNewLine("[AddElement]  QueueSize = " + std::to_string(m_Queue.size()));
        #endif
    }
    m_Notifier.notify_one();
}
////////////////////////////////////////////////////////////////////////////////
template<typename T>
uint64_t CThreadSafeQueue<T>::Size() const
{
    std::scoped_lock<std::mutex> guard(m_Locker);
    return m_Queue.size();
}
////////////////////////////////////////////////////////////////////////////////
template<typename T>
void CThreadSafeQueue<T>::SetPrimaryWaitTime(std::chrono::milliseconds PrimaryWaitTime)
 {
     m_PrimaryWaitTime = PrimaryWaitTime;
     m_WaitTime = m_PrimaryWaitTime;
 }
////////////////////////////////////////////////////////////////////////////////
template<typename T>
void CThreadSafeQueue<T>::WakeUpAllThreads()
 {
     m_StopWaitAll = true;
     m_Notifier.notify_all();
 }
////////////////////////////////////////////////////////////////////////////////
 template<typename T>
 void CThreadSafeQueue<T>::IncreaseWaitTime()
 {
     m_WaitTime = m_WaitTime * 2;
 }