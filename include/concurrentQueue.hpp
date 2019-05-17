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
	std::queue<T> _queue;
	std::mutex queueMutex;
	std::condition_variable queueCV;

	
public:
	ConcurrentQueue () {
		// TODO
	}

	~ConcurrentQueue () {
		// TODO
	}


	void pop (T& item) {
		std::unique_lock<std::mutex> lock (queueMutex);
		queueCV.wait (lock, [&]{return !_queue.empty();});
		
		item	= std::move (_queue.front());
		_queue.pop();
	}


	void push(const T& item) {
		std::unique_lock<std::mutex> lock (queueMutex);
		_queue.push (item);
		lock.unlock ();
		queueCV.notify_one ();
	}
};


#endif // CONCURRENT_QUEUE_HPP