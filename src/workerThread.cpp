/**
 * \file workerThread.cpp
 * \brief Implementation file for WorkerThread class
 * \author Luca Di Mauro
 */

#include <workerThread.hpp>

#include <stdexcept>


WorkerThread::WorkerThread () {
	stopMe			= false;
	taskPresence	= false;
	workerThread	= std::thread ([&] {
		workerFunction ();
	});
}




WorkerThread::WorkerThread (WorkerThread&& wt) {
	stopMe			= false;
	task			= std::move(wt.task);
	taskPresence	= wt.taskPresence;
	workerThread	= std::thread ([&] {
		workerFunction ();
	});
}




WorkerThread::~WorkerThread () {
	stopWorker ();

	if (workerThread.joinable ())
		workerThread.join ();
}




void WorkerThread::workerFunction () {
	Task localTask;

	while (!stopMe) {
		{
			std::unique_lock<std::mutex> lock (taskMutex);
			taskCV.wait (lock, [&] {return taskPresence==true || stopMe;});
			localTask		= std::move(task);
			taskPresence	= false;
		}

		if (!stopMe) {
			// Executing submitted activity
			localTask ();
		}
	}
}




void WorkerThread::assignActivity (Task a) {
	if (stopMe)
		return ;

	std::unique_lock<std::mutex> lock (taskMutex);
	
	taskCV.wait (lock, [&] {return taskPresence==false || stopMe;});
	if (!stopMe) {
		task			= std::move(a);
		taskPresence	= true;
	}

	taskCV.notify_all ();
}




void WorkerThread::stopWorker () {
	stopMe	= true;
	taskCV.notify_all ();
}




void WorkerThread::setAffinity (unsigned int idx) {
	cpu_set_t cpuset;
	CPU_ZERO (&cpuset);
	CPU_SET (idx, &cpuset);
	
	if (pthread_setaffinity_np (workerThread.native_handle(), sizeof(cpu_set_t), &cpuset) != 0) {
		throw std::logic_error ("Error setting affinity to processor  " + std::to_string(idx));
	}
}