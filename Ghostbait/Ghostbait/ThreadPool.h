#pragma once
#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <functional>

class ThreadPool {
	static int maxThreads;

	static std::vector<std::thread> pool;

	static std::queue<std::function<void()>> queue;

	static std::function<void()> job;

	static std::condition_variable condition;

	static std::mutex queueMutex;

	static void WaitForJob() {
		while(true) {
			std::unique_lock<std::mutex> lock(queueMutex);

			condition.wait(lock, [] { return !queue.empty(); });

			job = queue.front();
			queue.pop();

			job();
		}
	}

public:
	static void Start() {
		for(int i = 0; i < maxThreads; ++i) {
			pool.push_back(std::thread(WaitForJob));
		}
	}

	static void MakeJob(std::function<void()> _job) {
		std::unique_lock<std::mutex> lock(queueMutex);
		queue.push(_job);

		condition.notify_one();
	}
};
