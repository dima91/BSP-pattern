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



class WorkerThread {
public:
	// Defines the type of accepted tasks
	using Task	= std::packaged_task<void ()>;


private:
	std::thread workerThread;

	Task					task;
	bool					taskPresence;
	std::mutex				taskMutex;
	std::condition_variable	taskCV;

	std::atomic_bool	stopMe;
	bool				stopped;

	void workerFunction	();


public:
	WorkerThread	();
	WorkerThread	(WorkerThread &&);
	~WorkerThread	();

	void assignActivity		(Task a);
	void performActivity	();
	void stopWorker			();
	void setAffinity		(unsigned int idx);
};


#endif // WORKER_THREAD_HPP