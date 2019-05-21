/**
 * \file barrier.cpp
 * \brief Implementation file for Barrier class
 * \author Luca Di Mauro
 */

#include <barrier.hpp>

#include <stdexcept>


Barrier::Barrier (int activitiesNumber) {
	this->activitiesNumber	= activitiesNumber;
}




Barrier::~Barrier () {
	barrierCV.notify_all ();
}




void Barrier::decreaseBarrier () {
	std::unique_lock<std::mutex> lock (barrierMutex);

	if (activitiesNumber == 0)
		throw std::runtime_error ("Barrier aleady opened!");
	
	if (--activitiesNumber == 0)
		barrierCV.notify_all ();
	else
		barrierCV.wait (lock, [&] {return activitiesNumber == 0;});
}




void Barrier::reset (int activitiesNumber) {
	if (this->activitiesNumber != 0)
		throw std::runtime_error ("Activities number differs from 0!");
	
	std::unique_lock<std::mutex> lock (barrierMutex);
	this->activitiesNumber	= activitiesNumber;
	barrierCV.notify_all ();
}




void Barrier::waitForFinish () {
	std::unique_lock<std::mutex> lock (barrierMutex);
	barrierCV.wait (lock, [&] {return activitiesNumber == 0;});
}




int Barrier::getRemainingEntities () {
	std::unique_lock<std::mutex> lock (barrierMutex);
	return activitiesNumber;
}