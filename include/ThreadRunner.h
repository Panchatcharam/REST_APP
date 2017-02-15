#pragma once
#include <iostream>
#include <thread>
#include <vector>

typedef std::function<void ()> RunningFunc;

class ThreadRunner
{
private:
	ThreadRunner(const ThreadRunner & rhs) = delete;
	ThreadRunner(ThreadRunner && rhs) = delete;
	ThreadRunner & operator=(ThreadRunner && rhs) = delete;
	ThreadRunner & operator=(const ThreadRunner & rhs) = delete;
	std::vector<std::thread> threads;

public:
	ThreadRunner() = delete;
	ThreadRunner(unsigned int threadCount, RunningFunc func);
	~ThreadRunner();
	void Join();
};
