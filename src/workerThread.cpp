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




void WorkerThread::performActivity () {
	// TODO
}




void WorkerThread::stopWorker () {
	stopMe	= true;
	taskCV.notify_all ();
}