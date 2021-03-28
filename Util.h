#pragma once

#include <cstdint>

#include <thread>
#include <functional>
#include <unordered_map>

#include "ScopedThread.h"

namespace thread_pool
{
	using TaskID = unsigned long long;
	enum class PoolStatus
	{
		Stopped,
		Configured,
		Running,
		RunningButDoNotAcceptNewTasks
	};
	using Task = std::function<void()>;
	using ThreadID = std::thread::id;
	using ThreadMap = std::unordered_map<ThreadID, std::thread>;
	using IncreaseThreads = std::function<uint16_t(uint64_t)>;
}