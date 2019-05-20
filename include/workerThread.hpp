/**
 * \file workerThread.hpp
 * \brief Header file for WorkerThread class
 * \author Luca Di Mauro
 */


#ifndef WORKER_THREAD_HPP
#define WORKER_THREAD_HPP

#include <concurrentQueue.hpp>
#include <barrier.hpp>

#include <thread>
#include <vector>
#include <functional>
#include <future>
#include <atomic>
#include <memory>
#include <iostream>		// TODO Remove me after tests



class WorkerThread {
public:
	// Defines the type of accepted tasks
	using Task	= std::packaged_task<void ()>;


private:
	std::thread workerThread;

	std::shared_ptr<Task>	task;
	std::mutex				taskMutex;
	std::condition_variable	taskCV;

	std::atomic_bool	stopMe;
	bool				stopped;

	void workerFunction	();


public:
	WorkerThread	();
	~WorkerThread	();

	void assignActivity		(std::shared_ptr<Task> a);
	void performActivity	();
	void stopWorker			();
};




/* ==========  ==========  ==========  ==========  ========== */
/* ==========  ==========  ==========  ==========  ========== */




WorkerThread::WorkerThread () {
	stopMe			= false;
	task			= nullptr;
	workerThread	= std::thread ([&] {
		workerFunction ();
	});
}




WorkerThread::~WorkerThread () {
	if (workerThread.joinable ())
		workerThread.join ();
}




void WorkerThread::workerFunction () {
	std::shared_ptr<Task> localTask	= nullptr;

	while (!stopMe) {
		{
			std::unique_lock<std::mutex> lock (taskMutex);
			taskCV.wait (lock, [&] {return task!=nullptr || stopMe;});
			localTask	= std::move (task);
			task		= nullptr;
		}

		if (!stopMe) {
			// Executing submitted activity
			std::future<void> taskFuture	= localTask->get_future ();
			(*localTask) ();
		}
	}
}




void WorkerThread::assignActivity (std::shared_ptr<Task> a) {
	if (stopMe)
		return ;

	std::unique_lock<std::mutex> lock (taskMutex);
	if (task != nullptr)
		throw std::runtime_error ("An already assigned activity exists!");
	taskCV.wait (lock, [&] {return task==nullptr || stopMe;});
	task	= std::move(a);

	taskCV.notify_all ();

	std::cout << "Assigned\n" << std::endl;
}




void WorkerThread::performActivity () {
	// TODO
}




void WorkerThread::stopWorker () {
	stopMe	= true;
	taskCV.notify_all ();
}


#endif // WORKER_THREAD_HPP