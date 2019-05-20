/**
 * \file workerThread.hpp
 * \brief Header file for WorkerThread class
 * \author Luca Di Mauro
 */


#ifndef WORKER_THREAD_HPP
#define WORKER_THREAD_HPP

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
	WorkerThread	(WorkerThread &&);
	~WorkerThread	();

	void assignActivity		(std::shared_ptr<Task> a);
	void performActivity	();
	void stopWorker			();
};


#endif // WORKER_THREAD_HPP