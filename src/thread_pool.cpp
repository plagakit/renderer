#include "thread_pool.h"

#include <iostream>

void ThreadPool::Start()
{
	const auto numThreads = std::thread::hardware_concurrency();
	std::cout << "Num threads: " << numThreads << std::endl;

	for (auto i = 0; i < numThreads; i++)
		threads.emplace_back(std::thread(&ThreadPool::ThreadLoop, this));
}

void ThreadPool::QueueJob(const Job& job)
{
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		jobs.push(job);
	}
	mutexCondition.notify_one();
}

// used in a while loop, such that the main thread can wait the threadpool 
// to complete all the tasks before calling the threadpool destructor
bool ThreadPool::Busy()
{
	bool poolBusy;
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		poolBusy = !jobs.empty();
	}
	return poolBusy;
}

void ThreadPool::Stop()
{
	{
		std::unique_lock<std::mutex> lock(queueMutex);
		shouldTerminate = true;
	}
	mutexCondition.notify_all();

	for (std::thread& activeThread : threads)
		activeThread.join();
	threads.clear();
}

void ThreadPool::ThreadLoop()
{
	while (true)
	{
		Job job;
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			mutexCondition.wait(lock, [this] {
				return !jobs.empty() || shouldTerminate;
			});
			if (shouldTerminate)
				return;

			job = jobs.front();
			jobs.pop();
		}
		job();
	}
}