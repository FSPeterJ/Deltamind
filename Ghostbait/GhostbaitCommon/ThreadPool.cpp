#include "ThreadPool.h"
#include "Console.h"

namespace Threadding {

	int ThreadPool::maxThreads = std::thread::hardware_concurrency();

	int ThreadPool::maxNonPriorityThreads = (int)(std::thread::hardware_concurrency() * 0.75f);

	int ThreadPool::runningNonPriorityThreads = 0;

	bool ThreadPool::quit = false;

	bool ThreadPool::isAcceptingJobs = false;

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

				//Common::Console::consoleMutex.lock();
				//Common::Console::WarningLine << "PRIORITY JOBS: " << priorityJobs.size() << "  REGULAR JOBS: " << queue.size();
				//Common::Console::consoleMutex.unlock();

				job = std::move(priorityJobs.front());
				priorityJobs.pop();
				lock.unlock();
				try {
					job();
				}
				catch (...) {
					Common::Console::consoleMutex.lock();
					Common::Console::ErrorLine << "THREAD: PRIORITY JOB CRASH!!!";
					Common::Console::consoleMutex.unlock();
				}
			}
			else {
				if (!(runningNonPriorityThreads < maxNonPriorityThreads)) continue;

				job = std::move(queue.front());
				queue.pop();
				++runningNonPriorityThreads;
				lock.unlock();

				try {
					job();
				}
				catch (...) {
					Common::Console::consoleMutex.lock();
					Common::Console::ErrorLine << "THREAD: REGULAR JOB CRASH!!!";
					Common::Console::consoleMutex.unlock();
				}

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

	void ThreadPool::ClearQueues() {
		queueMutex.lock();


		while(!priorityJobs.empty()) {
			priorityJobs.pop();
		}
		while(!queue.empty()) {
			queue.pop();
		}

		Common::Console::consoleMutex.lock();
		Common::Console::ErrorLine << "ALL JOB QUEUES CLEARED!";
		Common::Console::consoleMutex.unlock();

		queueMutex.unlock();
	}

	void ThreadPool::AcceptNonCriticalJobs(bool _isAccept) {
		isAcceptingJobs = _isAccept;

		Common::Console::consoleMutex.lock();
		isAcceptingJobs ? Common::Console::WarningLine << "QUEUE IS OPEN FOR ALL JOBS." : Common::Console::WarningLine << "ACCEPTING PRIORITY JOBS ONLY!";
		Common::Console::consoleMutex.unlock();
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
