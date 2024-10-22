#pragma once

#include <functional>
#include <mutex>
#include <vector>
#include <queue>

using Job = std::function<void()>;

class ThreadPool {

public:
	void Start();
	void QueueJob(const Job& job);
	void Stop();
	bool Busy();

private:
	void ThreadLoop();

	bool shouldTerminate = false;			// tells threads to stop looking for jobs
	std::mutex queueMutex;					// prevents data races to job queue
	std::condition_variable mutexCondition;	// lets threads wait on new jobs

	std::vector<std::thread> threads;
	std::queue<Job> jobs;

};