#pragma once
#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <future>

class ThreadPool {
	static int maxThreads;

	static bool quit;

	static std::vector<std::thread> pool;

	using Job = std::packaged_task<void()>;

	static std::queue<Job> queue;

	static std::condition_variable condition;

	static std::mutex queueMutex;

	static void WaitForJob();

public:
	static void Start();

	template<typename F, typename... Args>
	static std::future<void> MakeJob(F f, Args&&... args) {
		Job task(std::bind(f, std::forward<Args>(args)...));
		auto fut = task.get_future();
		{
			//unique_lock is heavier to construct?
			//This should be unlocked immediately after pushing, but which is better?
			// Research futher, but this is probably irrelevant levels of performance
			std::lock_guard<std::mutex> lock(queueMutex);
			queue.push(std::move(task));
		}
		condition.notify_one();
		return fut;
	}
	static void Shutdown();
};
