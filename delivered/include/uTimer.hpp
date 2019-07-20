/**
 * \file uTimer.hpp
 * \brief Header file for UTimer class
 * \author Luca Di Mauro
 */

#ifndef U_TIMER_HPP
#define U_TIMER_HPP

#include <iostream>
#include <chrono>
#include <mutex>


/* Class which allows to keep track of elapsed time between creation of an object instance of this class and its destruction,
 * printing out a message registered during creation of object
 */
class UTimer {
private:
	using usecs		= std::chrono::microseconds;
	using msecs		= std::chrono::milliseconds;
	using TimePoint	= std::chrono::system_clock::time_point;
	
	TimePoint start;
	TimePoint stop;
	std::chrono::duration<double> elapsedTime;
	std::string message;
	

public:
	UTimer (const std::string m);

	~UTimer ();
};


#endif // U_TIMER_HPP