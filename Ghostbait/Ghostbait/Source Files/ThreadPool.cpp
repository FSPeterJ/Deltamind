#include "ThreadPool.h"

int ThreadPool::maxThreads = std::thread::hardware_concurrency();
//int ThreadPool::maxThreads = 1;

bool ThreadPool::quit = false;

std::vector<std::thread> ThreadPool::pool;

std::queue<ThreadPool::Job> ThreadPool::queue;

std::mutex ThreadPool::queueMutex;

std::condition_variable ThreadPool::condition;
