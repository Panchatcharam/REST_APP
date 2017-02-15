/*
 * ThreadRunner.cpp
 *
 *  Created on: Feb 2, 2017
 *      Author: developer
 */
#include "ThreadRunner.h"


ThreadRunner::ThreadRunner(unsigned int threadCount, RunningFunc func) : threads{}
{
	//std::cout<< " ThreadRunner::ThreadRunner()" << std::endl;
	threads.reserve(threadCount);

	for( unsigned int idx = 0; idx < threadCount; ++idx )
	{
		threads.emplace_back(func);
	}
}

ThreadRunner::~ThreadRunner()
{
	//std::cout<< " ThreadRunner::~ThreadRunner()" << std::endl;
}

void ThreadRunner::Join()
{
	for(std::thread & t : threads)
	{
		if (t.joinable())
		{
			t.join();
		}
	}
}
