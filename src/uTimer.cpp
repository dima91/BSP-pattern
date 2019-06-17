/**
 * \file uTimer.cpp
 * \brief Implementation file for UTimer class
 * \author Luca Di Mauro
 */

#include <uTimer.hpp>


UTimer::UTimer (const std::string m) : message(m) {
	start		= std::chrono::high_resolution_clock::now();
}




UTimer::~UTimer () {
	stop		= std::chrono::high_resolution_clock::now();
	elapsedTime	= stop - start;
	auto usec	= std::chrono::duration_cast<usecs>(elapsedTime).count();
	auto msec	= std::chrono::duration_cast<msecs>(elapsedTime).count();
	
	std::cout << message << " computed in " << usec << " usec  (" << msec << " milliseconds)" << std::endl;
}