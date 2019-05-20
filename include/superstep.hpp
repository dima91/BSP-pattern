/**
 * \file superstep.hpp
 * \brief Header file for Superstep class
 * \author Luca Di Mauro
 */


#ifndef SUPERSTEP_HPP
#define SUPERSTEP_HPP

#include <workerThread.hpp>
#include <barrier.hpp>



template<typename T>
class Superstep {
public:
	/* Defines the type of function which will be executed during the "communication phase" of BSP pattern.
	 * It requires the type T input of the activity and returns a vector of integers, on which the i-th position of
	 * vector contains the index of target activity for input element i
	 */
	using CommunicationProtocol	= std::vector<int>;

	// Defines the type of a general activity
	using ActivityFunction		= std::function<T& (T&)>;


private:


public:
	Superstep (/*activityFunction, parallelItems, communicationProtocol*/);
	~Superstep ();

	/* Method to execute this current superstep. It requries thread of workers on which the activity will be run,
	 * 
	 */
	/*std::future<int> runStep (std::vector<std::thread> &workers,
								std::concurrentQueue<std::vector<T>> input,
								std::ConcurrentQueue<std::vector<T>> output);*/

	//int getActivitiesNumber ();
};


#endif // SUPERSTEP_HPP