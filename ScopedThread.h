#pragma once

#include <thread>

namespace utils
{
	class CScopedThread {
	public:
		using ThreadID = std::thread::id;
		template<class ...Args>
		explicit CScopedThread(Args &&...args) : m_InternalThread(std::forward<Args>(args)...) {}

		CScopedThread(CScopedThread &&other) noexcept {
			m_InternalThread = std::move(other.m_InternalThread);
		}

		CScopedThread &operator=(CScopedThread &&other) noexcept {
			m_InternalThread = std::move(other.m_InternalThread);
			return *this;
		}

		std::thread &operator*() {
			return m_InternalThread;
		}

		std::thread const &operator*() const {
			return m_InternalThread;
		}

		std::thread *operator->() {
			return &operator*();
		}

		std::thread const *operator->() const {
			return &operator*();
		}

		auto get_id() const {
			return m_InternalThread.get_id();
		}

		auto join() {
			if (m_InternalThread.joinable())
			{
				m_InternalThread.join();
			}
		}

		~CScopedThread() {
			join();
		}

	private:
		std::thread m_InternalThread;
	};
}
