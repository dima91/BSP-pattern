/**
 * \file concurrentQueue.hpp
 * \brief Definition and implementation of ConcurrentQueue class
 * \author Luca Di Mauro
 */


#ifndef CONCURRENT_QUEUE_HPP
#define CONCURRENT_QUEUE_HPP

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>



template <typename T>
class ConcurrentQueue {
private:
	std::queue<T> &dataQueue;
	std::mutex queueMutex;
	std::condition_variable queueCV;

	
public:
	ConcurrentQueue (std::queue<T> &externalQueue) : dataQueue(externalQueue) {
	}

	~ConcurrentQueue () {
		// TODO
	}


	bool isEmpty () {
		std::unique_lock<std::mutex> lock (queueMutex);
		return dataQueue.empty ();
	}


	void pop (T& item) {
		std::unique_lock<std::mutex> lock (queueMutex);
		queueCV.wait (lock, [&]{return !dataQueue.empty();});
		
		item	= std::move (dataQueue.front());
		dataQueue.pop();
	}


	void push(const T& item) {
		std::unique_lock<std::mutex> lock (queueMutex);
		dataQueue.push (item);
		lock.unlock ();
		queueCV.notify_one ();
	}
};


#endif // CONCURRENT_QUEUE_HPP