#include "ThreadPool.h"


int ThreadPool::maxThreads = std::thread::hardware_concurrency();

std::vector<std::thread> ThreadPool::pool;

std::queue<std::function<void()>> ThreadPool::queue;

std::mutex ThreadPool::queueMutex;

std::function<void()> ThreadPool::job;

std::condition_variable ThreadPool::condition;
