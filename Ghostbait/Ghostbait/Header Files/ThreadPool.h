#pragma once
#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <functional>
#include <future>

class ThreadPool {
	static int maxThreads;

	static bool quit;

	static std::vector<std::thread> pool;

	using Job = std::packaged_task<void()>;

	static std::queue<Job> queue;

	static std::condition_variable condition;

	static std::mutex queueMutex;

	static void WaitForJob() {
		while(true) {
			// See comments below on which is better
			std::unique_lock<std::mutex> lock(queueMutex);
			condition.wait(lock, [] { return !queue.empty() || quit; });
			if(quit) {
				lock.unlock();
				break;
			}
			Job job = std::move(queue.front());
			queue.pop();
			// We are done with the queue, can release now
			lock.unlock();
			job();
		}
	}

public:
	static void Start() {
		for(int i = 0; i < maxThreads; ++i) {
			pool.push_back(std::thread(WaitForJob));
		}
	}

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
	static void Shutdown() {
		quit = true;
		condition.notify_all();
		for(auto& thread : pool) {
			if(thread.joinable()) {
				thread.join();
			}
		}
	}
};
