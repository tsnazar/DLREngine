#include "ParallelExecutor.h"

#define BREAK __debugbreak();

#define ALWAYS_ASSERT(expression, ...) \
	if(!(expression)) \
	{ \
	 BREAK\
	 std::abort; \
	}

#ifndef NDEBUG
#define DEV_ASSERT(...)
#else
#define DEV_ASSERT(expression, ...) ALWAYS_ASSERT(expression, __VA_ARGS__);
#endif // !NDEBUG

ParallelExecutor::ParallelExecutor(unsigned int numThreads)
{
	m_IsLooping = true;

	DEV_ASSERT(numThreads);
	m_FinishedThreadNum = 0;

	m_Threads.reserve(numThreads);
	for (unsigned int i = 0; i < numThreads; ++i)
		m_Threads.emplace_back([this, i]() {Loop(i); });
}

ParallelExecutor::~ParallelExecutor()
{
	Wait();
	m_IsLooping = false;
	Awake();

	for (auto& t : m_Threads) t.join();
}

void ParallelExecutor::Wait()
{
	if (!IsWorking()) return;

	std::unique_lock<std::shared_mutex> lock(m_Mutex);
	if (!IsWorking()) return;

	m_WaitCV.wait(lock);
}

void ParallelExecutor::Execute(const Func& func, unsigned int numTasks, unsigned int tasksPerBatch)
{
	if (numTasks == 0) return;
	ExecuteAsync(func, numTasks, tasksPerBatch);
	Wait();
}

void ParallelExecutor::ExecuteAsync(const Func& func, unsigned int numTasks, unsigned int tasksPerBatch)
{
	if (numTasks == 0) return;
	DEV_ASSERT(tasksPerBatch > 0);

	Wait();

	m_FinishedThreadNum = 0;
	m_CompletedBatchNum = 0;

	unsigned int numBatches = (numTasks + tasksPerBatch - 1) / tasksPerBatch;

	m_ExecuteTasks = [this, func, numTasks, numBatches, tasksPerBatch](unsigned int threadIndex)
	{
		while (true)
		{
			unsigned int batchIndex = m_CompletedBatchNum.fetch_add(1);
			if (batchIndex >= numBatches) return;

			unsigned int begin = (batchIndex + 0) * tasksPerBatch;
			unsigned int end = (batchIndex + 1) * tasksPerBatch;
			if (end > numTasks) end = numTasks;

			for (unsigned int taskIndex = begin; taskIndex < end; ++taskIndex)
				func(threadIndex, taskIndex);
		}
	};

	Awake();
}

void ParallelExecutor::Awake()
{
	m_WorkCV.notify_all();
}

void ParallelExecutor::Loop(unsigned int threadIndex)
{
	while (true)
	{
		{
			std::shared_lock<std::shared_mutex> lock(m_Mutex);

			unsigned int prevFinishedNum = m_FinishedThreadNum.fetch_add(1);
			if ((prevFinishedNum + 1) == m_Threads.size())
			{
				m_ExecuteTasks = {};
				m_WaitCV.notify_all();
			}
			m_WorkCV.wait(lock);
		}

		if (!m_IsLooping) return;

		DEV_ASSERT(m_ExecuteTasks);
		m_ExecuteTasks(threadIndex);
	}
}
