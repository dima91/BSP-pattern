#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <ctime>


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
	UTimer (const std::string m) : message(m) {
		start = std::chrono::system_clock::now();
	}

	~UTimer () {
		stop		= std::chrono::system_clock::now();
		elapsedTime	= stop - start;
		auto musec	= std::chrono::duration_cast<usecs>(elapsedTime).count();
		
		std::cout << message << " computed in " << musec << " usec " << std::endl;

	}
};