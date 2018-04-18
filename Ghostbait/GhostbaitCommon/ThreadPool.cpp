#include "ThreadPool.h"

namespace Threadding {

	int ThreadPool::maxThreads = std::thread::hardware_concurrency();

	int ThreadPool::maxNonPriorityThreads = (int)(std::thread::hardware_concurrency() * 0.70f);

	int ThreadPool::runningNonPriorityThreads = 0;

	bool ThreadPool::quit = false;

	std::vector<std::thread*> ThreadPool::pool;

	std::queue<ThreadPool::Job> ThreadPool::queue;

	std::queue<ThreadPool::Job> ThreadPool::priorityJobs;

	std::mutex ThreadPool::queueMutex;

	std::condition_variable ThreadPool::condition;

	void ThreadPool::WaitForJob() {
		while(true) {
			// See comments below on which is better
			std::unique_lock<std::mutex> lock(queueMutex);
			condition.wait(lock, [] { return !priorityJobs.empty() || !queue.empty() || quit; });
			if(quit) {
				lock.unlock();
				break;
			}

			Job job;
			if (!priorityJobs.empty()) {
				job = std::move(priorityJobs.front());
				priorityJobs.pop();
				lock.unlock();
				job();
			}
			else {
				if (!(runningNonPriorityThreads < maxNonPriorityThreads)) continue;

				job = std::move(queue.front());
				queue.pop();
				++runningNonPriorityThreads;
				lock.unlock();
				job();

				lock.lock();
				--runningNonPriorityThreads;
				lock.unlock();
			}
			// We are done with the queue, can release now
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
