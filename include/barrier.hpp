/**
 * \file barrier.hpp
 * \brief Header file for Barrier class
 * \author Luca Di Mauro
 */


#ifndef BARRIER_HPP
#define BARRIER_HPP

#include <mutex>
#include <condition_variable>



class Barrier {
private:
	std::mutex barrierMutex;
	std::condition_variable barrierCV;


public:
	Barrier (int activitiesNumber);
	~Barrier ();

	//void wait ();
	//void decreaseRunningActivities ();
};


#endif // BARRIER_HPP