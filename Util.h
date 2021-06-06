#pragma once

#include <cstdint>
#include <atomic>
#include <thread>
#include <functional>
#include <unordered_map>

namespace tp
{
 
	enum class WaitResult : std::uint8_t
	{
		GotElement,
		Timeout,
		WakeUpAll
	};

	enum class PoolStatus : std::uint8_t
	{
		Configured,
		Running,
		Stopped
	};
	using ThreadID = std::thread::id;
	using ThreadHashMap = std::unordered_map<ThreadID, std::thread>;
	using IncreaseThreads = std::function<size_t(size_t)>;


#define SYNCHRONIZED(m, code) { \
		std::scoped_lock<std::mutex> lock(m); \
			code \
		}

	#define SYNCHRONIZED_INVOCABLE(m, code) std::invoke([this](){ \
		std::scoped_lock<std::mutex> lock(m); \
			return code; \
		})

}