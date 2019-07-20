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
public:
	using usecs		= std::chrono::microseconds;
	using msecs		= std::chrono::milliseconds;
private:
	using TimePoint	= std::chrono::system_clock::time_point;
	
	TimePoint start;
	TimePoint stop;
	std::chrono::duration<double> elapsedTime;
	std::string message;
	bool printMessage;
	

public:
	UTimer (const std::string m, bool printAtDestruction=true);

	~UTimer ();

	template<typename T>
	T getElapsedTime () {
		stop		= std::chrono::high_resolution_clock::now();
		return std::chrono::duration_cast<T> (stop-start);
	}
};


#endif // U_TIMER_HPP