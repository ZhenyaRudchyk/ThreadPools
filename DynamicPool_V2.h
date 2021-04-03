//#pragma once
//
//#include <cstdint>
//
//#include <atomic>
//#include <future>
//
//#include "Util.h"
//#include "Logger.h"
//#include "DobleQueueContainer.h"
//#include "Task.h"
//
//namespace thread_pool
//{
//
//    class CDynamicPoolV2
//    {
//    public:
//        CDynamicPoolV2() :
//            m_fpIncreaseThreadsAlgorithm(nullptr),
//            m_PoolStatus(PoolStatus::Stopped),
//            m_ui16MaxWorkers(0),
//            m_ui16MinWorkers(0),
//            m_ui16DestroyWorkersCounter(0)
//        {}
//        ~CDynamicPoolV2();
//
//        bool Initialize(const uint16_t ui16MinWorkers,
//            const uint16_t ui16MaxWorkers,
//            std::chrono::milliseconds WaitTime,
//            IncreaseThreads fpIncreaseThreadsAlgorithm);
//        bool Start();
//        void Stop(); // join threads only when queue is empty
//        void EmergencyStop(); // join threads right now do not wait utill queue with tasks is empty 
//        template <typename TCallable, typename ... TArgs>
//        void AddTaskWithoutResult(TCallable&& func, TArgs &&... args);
//
//        // template <typename TCallable, typename ... TArgs>
//           //auto AddTaskWithFutureResult(TCallable &&func, TArgs &&... args) -> std::future<typename std::result_of<TCallable(TArgs...)>::type>;
//    protected:
//        void Run();
//        bool CreateWorker();
//        void DestroyWorker(const ThreadID threadToDestory);
//        void WaitUntilQueueEmpty();
//    private:
//        ThreadMap m_Workers;
//        std::mutex m_WorkersMutex;
//        uint16_t m_ui16DestroyWorkersCounter; // counter for threads which is not destroyed yet but already submited for that
//        uint16_t m_ui16MaxWorkers;
//        uint16_t m_ui16MinWorkers;
//        // return: number of threads increase to 1)Queue Size
//        IncreaseThreads m_fpIncreaseThreadsAlgorithm; // executes under multithreaded code. Take care about synchronization by yourself;
//        std::atomic<PoolStatus> m_PoolStatus;
//        ThreadSafeQueueV2<Task> m_TaskQueue;
//        std::condition_variable m_Waiter; // wait until queue empty
//        std::mutex m_WaiterMutex;
//        //TaskID m_TaskID
//    };
//
//
//    CDynamicPoolV2::~CDynamicPoolV2()
//    {
//#ifdef LOG
//        g_Logger.WriteToFileWithNewLine("[CDynamicPoolV2::~CDynamicPoolV2] Workers = " + std::to_string(m_Workers.size()) + " Queue size =" + std::to_string(m_TaskQueue.Size()));
//#endif 
//        if (m_PoolStatus != PoolStatus::Stopped)
//        {
//            Stop();
//        }
//    }
//    /////////////////////////////////////////////////////////////////////////////////////
//    bool CDynamicPoolV2::CreateWorker()
//    {
//        if (m_PoolStatus != PoolStatus::Stopped)
//        {
//            if (m_Workers.size() >= m_ui16MaxWorkers)
//            {
//                return false;
//            }
//            std::thread tempThread(&CDynamicPoolV2::Run, this);
//            m_Workers[tempThread.get_id()] = std::move(tempThread);
//#ifdef LOG
//            g_Logger.WriteToFileWithNewLine("[CreateWorker] Workers = " + std::to_string(m_Workers.size()) + " " + ThreadIDToString(std::this_thread::get_id()));
//#endif  
//            return true;
//        }
//
//        return false;
//    }
//    /////////////////////////////////////////////////////////////////////////////////////
//    void CDynamicPoolV2::DestroyWorker(const ThreadID IdThreadToDestory)
//    {
//        if (m_PoolStatus != PoolStatus::Stopped)
//        {
//            m_TaskQueue.push_back(
//                [this, IdThreadToDestory]() {
//                    std::unique_lock<std::mutex> guard(m_WorkersMutex);
//                    m_Workers[IdThreadToDestory].join();
//                    m_Workers.erase(IdThreadToDestory);
//                    guard.unlock();
//                    --m_ui16DestroyWorkersCounter;
//#ifdef LOG
//                    g_Logger.WriteToFileWithNewLine("[DestroyWorker] Workers = " + std::to_string(m_Workers.size()) + " remained");
//#endif  
//                });
//        }
//    }
//    /////////////////////////////////////////////////////////////////////////////////////
//    void CDynamicPoolV2::Stop()
//    {
//        m_PoolStatus.store(PoolStatus::RunUntilQueueEmptyAndDoNotAcceptNewTasks);
//        WaitUntilQueueEmpty();
//        m_PoolStatus.store(PoolStatus::Stopped);
//        m_TaskQueue.WakeUpAllThreads();
//        std::for_each(m_Workers.begin(), m_Workers.end(), [](ThreadMap::value_type& x) { (x.second).join(); });
//        m_Workers.clear();
//    }
//    /////////////////////////////////////////////////////////////////////////////////////
//    bool CDynamicPoolV2::Start()
//    {
//        if (m_PoolStatus == PoolStatus::Configured)
//        {
//            for (uint16_t i = 0; i < m_ui16MinWorkers; ++i)
//            {
//                if (!CreateWorker())
//                {
//                    return false;
//                }
//            }
//            return true;
//        }
//        return false;
//    }
//    /////////////////////////////////////////////////////////////////////////////////////
//    inline void CDynamicPoolV2::EmergencyStop()
//    {
//        m_PoolStatus = PoolStatus::Stopped;
//        m_TaskQueue.WakeUpAllThreads();
//#ifdef LOG
//        g_Logger.WriteToFileWithNewLine("[EmergencyStop]  Queue size =" + std::to_string(m_TaskQueue.Size()) + " Workers = " + std::to_string(m_Workers.size()));
//#endif
//        std::for_each(m_Workers.begin(), m_Workers.end(), [](ThreadMap::value_type& x) { (x.second).join(); });
//        m_Workers.clear();
//    }
//    /////////////////////////////////////////////////////////////////////////////////////
//    void CDynamicPoolV2::Run()
//    {
//        if (m_PoolStatus != PoolStatus::Running)
//        {
//            m_PoolStatus = PoolStatus::Running;
//        }
//
//        while (m_PoolStatus != PoolStatus::Stopped)
//        {
//            std::unique_ptr<Task> taskToExecute = nullptr;
//            auto getResult = m_TaskQueue.pop(taskToExecute);
//            if (getResult == thread_pool::WaitResult::Timeout)
//            {
//                std::unique_lock<std::mutex> guard(m_WorkersMutex);
//                if (m_Workers.size() - m_ui16DestroyWorkersCounter > m_ui16MinWorkers)
//                {
//#ifdef LOG
//                    g_Logger.WriteToFileWithNewLine("[Run] TimeOut = " + ThreadIDToString(std::this_thread::get_id()) + " Workers = " + std::to_string(m_Workers.size()));
//#endif
//                    // thread is not useful destroy it
//                    DestroyWorker(std::this_thread::get_id());
//                    ++m_ui16DestroyWorkersCounter;
//                    break;
//                }
//                else
//                {
//                    guard.unlock();
//                    m_TaskQueue.IncreaseWaitTime();
//                }
//            }
//
//            if (getResult == thread_pool::WaitResult::GotElement)
//            {
//                if (taskToExecute)
//                {
//                    (*taskToExecute)();
//
//                    if (m_PoolStatus == PoolStatus::RunUntilQueueEmptyAndDoNotAcceptNewTasks && m_TaskQueue.Empty())
//                    {
//                        m_Waiter.notify_all();
//                    }
//                }
//            }
//        }
//    }
//    /////////////////////////////////////////////////////////////////////////////////////
//         //template <typename TCallable, typename ... TArgs>
//         //auto CDynamicPoolV2::AddTaskWithFutureResult(TCallable &&func, TArgs &&... args) -> std::future<typename std::result_of<TCallable(TArgs...)>::type>
//         //{
//         //   if(m_PoolStatus == PoolStatus::Running)
//         //   {
//         //   //get return type of the function
//         //     //typedef decltype(f(args...)) retType;
//         //      using PackedTask = std::packaged_task<typename std::result_of<TCallable(TArgs...)>::type()>;
//
//         //      auto task = std::make_shared<PackedTask>(std::bind(std::forward<TCallable>(func), std::forward<TArgs>(args)...));
//         //     //package the task
//         //      //std::packaged_task<retType()> task(std::move(std::bind(f, args...)));
//         //           //get the future from the task before the task is moved into the jobqueue
//         //     auto future = task->get_future();
//         //     // lock jobqueue mutex, add job to the job queue
//
//         //     //place the job into the queue
//         //     m_TaskQueue.AddElement([task](){ *task();});
//         //   
//         //     {
//         //        std::scoped_lock<std::mutex> guard(m_WorkersMutex);
//         //         int64_t i64ThreadsToAdd = m_fpIncreaseThreadsAlgorithm(m_Workers.size(),m_TaskQueue.Size()) - m_Workers.size();
//         //         if(i64ThreadsToAdd > 0)
//         //         {
//         //            for(int64_t i = 0; i < i64ThreadsToAdd; ++i)
//         //            {
//         //               if(!CreateWorker())
//         //               {
//         //                  break;
//         //               }
//         //            }
//         //         }  
//         //     }
//         //     //return the future for the function so the user can get the return value
//         //     return future;
//         //   }
//         //}
//    /////////////////////////////////////////////////////////////////////////////////////
//    template <typename TCallable, typename ... TArgs>
//    void CDynamicPoolV2::AddTaskWithoutResult(TCallable&& func, TArgs &&... args)
//    {
//        if (m_PoolStatus == PoolStatus::Running || m_PoolStatus == PoolStatus::Configured)
//        {
//            using PackedTask = std::packaged_task<typename std::invoke_result<TCallable, TArgs...>::type()>;
//            auto task = std::make_shared<PackedTask>(std::bind(std::forward<TCallable>(func), std::forward<TArgs>(args)...));
//            m_TaskQueue.push_back([task]() { (*task)();
//                });
//            int64_t i64ThreadsToIncreaseNumber = m_fpIncreaseThreadsAlgorithm(m_TaskQueue.Size());
//            {
//                std::scoped_lock<std::mutex> guard(m_WorkersMutex);
//                std::uint16_t ui16ThreadsPerfectSize = i64ThreadsToIncreaseNumber - m_Workers.size();
//                for (int64_t i = 0; i < ui16ThreadsPerfectSize; ++i)
//                {
//                    if (!CreateWorker())
//                    {
//                        break;
//                    }
//                }
//            }
//        }
//    }
//
//    /////////////////////////////////////////////////////////////////////////////////////
//    bool CDynamicPoolV2::Initialize(const uint16_t ui16MinWorkers,
//        const uint16_t ui16MaxWorkers,
//        std::chrono::milliseconds WaitTime,
//        IncreaseThreads fpIncreaseThreadsAlgorithm)
//    {
//        m_ui16MinWorkers = ui16MinWorkers;
//        m_ui16MaxWorkers = ui16MaxWorkers;
//        if (m_ui16MaxWorkers <= m_ui16MinWorkers || m_ui16MaxWorkers < 0 || m_ui16MinWorkers < 0)
//        {
//            return false;
//        }
//        m_TaskQueue.SetPrimaryWaitTime(WaitTime);
//        m_fpIncreaseThreadsAlgorithm = fpIncreaseThreadsAlgorithm;
//        m_PoolStatus = PoolStatus::Configured;
//        return true;
//    }
//    /////////////////////////////////////////////////////////////////////////////////////
//    void CDynamicPoolV2::WaitUntilQueueEmpty()
//    {
//        std::unique_lock<std::mutex> guard(m_WaiterMutex);
//        while (m_TaskQueue.Empty())
//        {
//            m_Waiter.wait(guard);
//        }
//    }
//
//}
//
