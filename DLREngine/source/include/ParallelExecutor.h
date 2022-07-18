#pragma once
#include <array>
#include <string>
#include <iostream>
#include <chrono>

#include <vector>
#include <atomic>
#include <functional>
#include <thread>
#include <shared_mutex>

struct ParallelExecutor
{
	static const unsigned int MAX_THREADS;

	static const unsigned int HALF_THREADS;

	using Func = std::function<void(unsigned int, unsigned int)>;

	ParallelExecutor(unsigned int numThreads);
	~ParallelExecutor();

	unsigned int GetNumThreads() const { return m_Threads.size(); }
	bool IsWorking() const { return m_FinishedThreadNum < m_Threads.size(); }

	void Wait();
	void Execute(const Func& func, unsigned int numTasks, unsigned int tasksPerBatch);
	void ExecuteAsync(const Func& func, unsigned int numTasks, unsigned int tasksPerBatch);

protected:
	void Awake();
	void Loop(unsigned int threadIndex);

	bool m_IsLooping;

	std::atomic<unsigned int> m_FinishedThreadNum;
	std::atomic<unsigned int> m_CompletedBatchNum;
	std::function <void(unsigned int)> m_ExecuteTasks;

	std::shared_mutex m_Mutex;
	std::condition_variable_any m_WaitCV;
	std::condition_variable_any m_WorkCV;
	
	std::vector<std::thread> m_Threads;
};

