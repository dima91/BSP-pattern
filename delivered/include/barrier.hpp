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



/* This class represents a barrier on which activities can suspend themselves until
 * condition 'activitiesNumber' reaches value 0.
 */
class Barrier {
private:
	std::atomic_int	activitiesNumber;

	std::mutex barrierMutex;
	std::condition_variable barrierCV;


public:
	Barrier (int activitiesNumber);
	~Barrier ();

	/* Method to decrease barrier of 1 and waiting other activities completion if there are running activities,
	 * waking up them otherwise
	 */
	void decreaseBarrier ();

	/* Method to wait until all the activities has finished their computation, namely until 'activitiesNumber' variable
	 * differs from 0
	 */
	void waitForFinish ();

	int getRemainingEntities ();
	void reset (int activitiesNumber);
};


#endif // BARRIER_HPP