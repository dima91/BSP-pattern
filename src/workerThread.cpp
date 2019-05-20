/**
 * \file workerThread.cpp
 * \brief Implementation file for WorkerThread class
 * \author Luca Di Mauro
 */

#include <workerThread.hpp>

#include <stdexcept>


WorkerThread::WorkerThread () {
	stopMe			= false;
	task			= nullptr;
	workerThread	= std::thread ([&] {
		workerFunction ();
	});
}




WorkerThread::WorkerThread (WorkerThread&& wt) {
	stopMe			= false;
	task			= wt.task;
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
}




void WorkerThread::performActivity () {
	// TODO
}




void WorkerThread::stopWorker () {
	stopMe	= true;
	taskCV.notify_all ();
}