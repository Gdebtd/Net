#ifndef NET_TSQUEUE_H
#define NET_TSQUEUE_H

#include "net_common.h"

namespace net {
	template <typename T>
	class tsqueue {
	public:
		tsqueue() = default;
		tsqueue(const tsqueue<T>&) = delete;
		virtual ~tsqueue() { clear(); }

	public:
		const T& front() {
			std::unique_lock<std::mutex> lock(muxQueue, std::try_to_lock);
			return deqQueue.front();
		}

		const T& back() {
			std::unique_lock<std::mutex> lock(muxQueue, std::try_to_lock);
			return deqQueue.back();
		}

		void push_back(const T& item) {
			std::unique_lock<std::mutex> lock(muxQueue, std::try_to_lock);
			deqQueue.emplace_back(std::move(item));
		}

		void push_front(const T& item) {
			std::unique_lock<std::mutex> lock(muxQueue, std::try_to_lock);
			deqQueue.emplace_front(std::move(item));
		}

		bool empty() {
			std::unique_lock<std::mutex> lock(muxQueue, std::try_to_lock);
			return deqQueue.empty();
		}

		size_t count() {
			std::unique_lock<std::mutex> lock(muxQueue, std::try_to_lock);
			return deqQueue.size();
		}

		void clear() {
			std::unique_lock<std::mutex> lock(muxQueue, std::try_to_lock);
			deqQueue.clear();
		}

		T pop_front() {
			std::unique_lock<std::mutex> lock(muxQueue, std::try_to_lock);
			auto t = std::move(deqQueue.front());
			deqQueue.pop_front();
			return t;
		}

		T pop_back() {
			std::unique_lock<std::mutex> lock(muxQueue, std::try_to_lock);
			auto t = std::move(deqQueue.back());
			deqQueue.pop_back();
			return t;
		}
	protected:
		std::mutex muxQueue;
		std::deque<T> deqQueue;
	};
}

#endif /* NET_TSQUEUE_H */