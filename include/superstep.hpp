/**
 * \file superstep.hpp
 * \brief Header file for Superstep class
 * \author Luca Di Mauro
 */


#ifndef SUPERSTEP_HPP
#define SUPERSTEP_HPP

#include <concurrentQueue.hpp>
#include <workerThread.hpp>
#include <barrier.hpp>



template<typename T>
class Superstep {
public:
	/* Defines the type of function which will be executed during the "communication phase" of BSP pattern.
	 * It requires the type T input of the activity and returns a vector of integers, on which the i-th position of
	 * vector contains the index of target activity for input element i
	 */
	using CommunicationProtocol	= std::vector<std::vector<int>>;

	// Defines the type of a general activity
	using ActivityFunction		= std::function<std::vector<T> (std::vector<T>&)>;


private:
	ActivityFunction activityFunction;
	int parallelismDegree;
	CommunicationProtocol commProto;
	Barrier barrier;

	void activityTask (std::vector<T> inputItems);


public:
	Superstep (ActivityFunction fun, int parDegree, CommunicationProtocol proto);
	~Superstep ();

	/* Method to execute this current superstep. It requries thread of workers on which the activity will be run,
	 * 
	 */
	int runStep (std::vector<WorkerThread> &workers,
					std::vector<ConcurrentQueue<T>> input,
					std::vector<ConcurrentQueue<T>> output);

	int getActivitiesNumber ();
};




/* ==========  ==========  ==========  ==========  ========== */
/* ==========  ==========  ==========  ==========  ========== */




template<typename T>
Superstep<T>::Superstep (ActivityFunction fun, int parDegree, CommunicationProtocol proto) : barrier(parDegree) {
	activityFunction	= fun;
	parallelismDegree	= parDegree;
	commProto			= proto;
}



template<typename T>
Superstep<T>::~Superstep () {
	// TODO
}




template<typename T>
void Superstep<T>::activityTask (std::vector<T> inputItems) {
	// Computation phase
	activityFunction (inputItems);


}




template<typename T>
int Superstep<T>::runStep (std::vector<WorkerThread> &workers,
							std::vector<ConcurrentQueue<T>> input,
							std::vector<ConcurrentQueue<T>> output) {
	// TODO
	
	// Setting up communication phase
	barrier.reset (parallelismDegree);

	return -1;
}


#endif // SUPERSTEP_HPP