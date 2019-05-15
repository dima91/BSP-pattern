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
private :
	std::queue<T> _queue;
	std::mutex queueMutex;
	std::condition_variable queueCV;
public :
	ConcurrentQueue () {
		// TODO
	}

	~ConcurrentQueue () {
		// TODO
	}

	T pop() {
		std::unique_lock<std::mutex>  lock (queueMutex);
		queueCV.wait (lock, [&]{return !_queue.empty();});
		
		auto val	= _queue.front();
		_queue.pop();
		
		return val;
	}


	/*void pop (T& item) {
	std::unique_lock<std::mutex> mlock(mutex_);
	while (queue_.empty())
	{
	cond_.wait(mlock);
	}
	item = queue_.front();
	queue_.pop();
	}*/


	void push(const T& item) {
		std::unique_lock<std::mutex> lock (queueMutex);
		_queue.push (item);
		lock.unlock ();
		queueCV.notify_one ();
	}
};


#endif // CONCURRENT_QUEUE_HPP