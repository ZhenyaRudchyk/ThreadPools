#pragma once

#include <cstdint>

#include <thread>
#include <functional>
#include <unordered_map>

namespace thread_pool
{
	using TaskID = unsigned long long;

	enum class WaitResult : std::uint8_t
	{
		GotElement,
		Timeout,
		WakeUpAll
	};

	enum class PoolStatus : std::uint8_t
	{
		Stopped,
		Configured,
		Running,
		RunUntilQueueEmptyAndDoNotAcceptNewTasks
	};
	using Task = std::function<void()>;
<<<<<<< Updated upstream
	using ThreadID = std::thread::id;
	using ThreadMap = std::unordered_map<ThreadID, std::thread>;
	using IncreaseThreads = std::function<uint16_t(uint64_t)>;
=======
	using ThreadMap = std::unordered_map<std::thread::id, std::thread>;
	using ThreadID = std::thread::id;
	using IncreaseThreads = std::function<std::uint16_t(std::uint64_t)>;
>>>>>>> Stashed changes
}