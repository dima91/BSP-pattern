#include <iostream>
#include <string>
#include <chrono>
#include <ctime>

#ifndef U_TIMER_HPP
#define U_TIMER_HPP


class UTimer {
private:
	using usecs		= std::chrono::microseconds;
	using msecs		= std::chrono::milliseconds;
	using TimePoint	= std::chrono::system_clock::time_point;
	
	TimePoint start;
	TimePoint stop;
	std::chrono::duration<double> elapsedTime;
	std::string message;

	static std::mutex *outputMutex;


public:
	UTimer (const std::string m) : message(m) {
		start		= std::chrono::high_resolution_clock::now();
		outputMutex	= nullptr;
	}

	~UTimer () {
		stop		= std::chrono::high_resolution_clock::now();
		elapsedTime	= stop - start;
		auto usec	= std::chrono::duration_cast<usecs>(elapsedTime).count();
		auto msec	= std::chrono::duration_cast<msecs>(elapsedTime).count();
		
		std::cout << message << " computed in " << usec << " usec  (" << msec << " milliseconds)" << std::endl;
	}
};

std::mutex *UTimer::outputMutex;


#endif // U_TIMER_HPP