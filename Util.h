#pragma once

#include <cstdint>
#include <atomic>
#include <thread>
#include <functional>
#include <unordered_map>
#include <utility>

#include "SkopedThread.h"

namespace tp
{
 
	enum class WaitResult : std::uint8_t
	{
		Unknown,
		GotElement,
		Timeout,
		Stop
	};

	using Task = std::function<void()>;
	using DynamicTask = std::pair<Task, bool>;
	using ThreadHashMapRAII = std::unordered_map<CScopedThread::ThreadID, CScopedThread>;
	using DynamicThreadHashMapRAII = std::unordered_map<CScopedThread::ThreadID, std::unique_ptr<CScopedThread>>;

}