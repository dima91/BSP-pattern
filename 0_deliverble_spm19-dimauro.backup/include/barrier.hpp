/**
 * \file barrier.hpp
 * \brief Header file for Barrier class
 * \author Luca Di Mauro
 */


#ifndef BARRIER_HPP
#define BARRIER_HPP

#include <mutex>
#include <condition_variable>
#include <atomic>



class Barrier {
private:
	std::atomic_int	activitiesNumber;

	std::mutex barrierMutex;
	std::condition_variable barrierCV;


public:
	Barrier (int activitiesNumber);
	~Barrier ();

	void decreaseBarrier ();
	void reset (int activitiesNumber);
	void waitForFinish ();
	int getRemainingEntities ();
};


#endif // BARRIER_HPP