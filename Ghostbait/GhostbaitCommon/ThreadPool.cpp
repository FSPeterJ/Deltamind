#include "ThreadPool.h"

namespace Threadding {

	int ThreadPool::maxThreads = std::thread::hardware_concurrency();
	//int ThreadPool::maxThreads = 1;

	bool ThreadPool::quit = false;

	std::vector<std::thread*> ThreadPool::pool;

	std::queue<ThreadPool::Job> ThreadPool::queue;

	std::mutex ThreadPool::queueMutex;

	std::condition_variable ThreadPool::condition;

	void ThreadPool::WaitForJob() {
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

	void ThreadPool::Start() {
		for(int i = 0; i < maxThreads; ++i) {
			pool.push_back(new std::thread(WaitForJob));
		}
	}

	void ThreadPool::Shutdown() {
		quit = true;
		condition.notify_all();
		for(auto& thread : pool) {
			if(thread->joinable()) {
				thread->join();
			}
			delete thread;
		}
	}

}
