#pragma once

#include "boost\asio.hpp"
#include "boost\thread.hpp"
#include "boost\smart_ptr.hpp"

#include <vector>

class ThreadPool{
public:
	ThreadPool();
	~ThreadPool();
	ThreadPool(size_t count);
	template <typename TFunc>
	void AddBatchTask(TFunc f);
	template <typename TFunc>
	void AddAsyncTask(TFunc f);
	void WaitBatch();

	boost::asio::io_service ioService;

private:

	boost::shared_ptr<boost::asio::io_service::work> work;
	boost::thread_group threads;
	boost::condition_variable taskCond;
	boost::mutex taskMutex;
	size_t taskCount;
	size_t threadCount;

	template <typename TFunc>
	void Callback(boost::tuple<TFunc> f);
};

ThreadPool::ThreadPool(size_t threadCount)
	: work(new boost::asio::io_service::work(ioService)),
	taskCount(0),
	threadCount(threadCount)
{
	taskMutex.initialize();	

	for (int i = 0; i < threadCount; i++){
		boost::thread* t = new boost::thread([](boost::asio::io_service* ioService){ ioService->run(); }, &ioService);
		threads.add_thread(t);
	}
}

ThreadPool::ThreadPool()
	: ThreadPool(boost::thread::hardware_concurrency())
{
}

ThreadPool::~ThreadPool()
{
	ioService.stop();
	threads.join_all();
}

void ThreadPool::WaitBatch(){
	boost::unique_lock<boost::mutex> lock(taskMutex);
	while (taskCount){
		taskCond.wait(lock);
	}
}

template<typename TFunc>
void ThreadPool::AddBatchTask(TFunc f){
	{
		boost::unique_lock<boost::mutex> lock(taskMutex);
		taskCount++;
	}

	void (ThreadPool::*ff)(boost::tuple<TFunc>) = &ThreadPool::Callback < TFunc > ;
	ioService.post(boost::bind(ff, this, boost::make_tuple(f)));
}

template<typename TFunc>
void ThreadPool::AddAsyncTask(TFunc f){
	ioService.post(f);
}

template<typename TFunc>
void ThreadPool::Callback(boost::tuple<TFunc> f){
	boost::get<0>(f)();
	{
		boost::unique_lock<boost::mutex> lock(taskMutex);
		taskCount--;
		taskCond.notify_one();
	}
}