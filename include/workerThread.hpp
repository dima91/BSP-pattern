/**
 * \file workerThread.hpp
 * \brief Header file for WorkerThread class
 * \author Luca Di Mauro
 */


#ifndef WORKER_THREAD_HPP
#define WORKER_THREAD_HPP

#include <thread>
#include <vector>
#include <functional>
#include <future>



class WorkerThread {
public:
	/*template <typename... Args>
	using Activity	= std::function<void (Args args...)>;*/


private:
	/*std::thread workerThread;
	std::mutex newActivityMutex;
	std::condition_variable activityCV;*/


public:
	WorkerThread () {
		// TODO
	}


	~WorkerThread () {
		// TODO
	}


	template <typename... Args>
	void assignActivity (std::function<void (Args...)> &a, Args... args) {
	}
};


#endif // WORKER_THREAD_HPP