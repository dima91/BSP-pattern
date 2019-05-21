/**
 * \file superstep.hpp
 * \brief Header file for Superstep class
 * \author Luca Di Mauro
 */


#ifndef SUPERSTEP_HPP
#define SUPERSTEP_HPP

#include <workerThread.hpp>
#include <barrier.hpp>
#include <lockableVector.hpp>

#include <iostream>		// REMOVE ME AFTER TESTS



template<typename T>
class Superstep {
public:
	/* Defines the type of function which will be executed during the "communication phase" of BSP pattern.
	 * The i-th position of the vector contains indexes of elements which will be sent to activity x
	 * of next superstep (length of this vector should be equal to parallelism degree)
	 */
	using CommunicationProtocol	= std::vector<std::vector<int>>;

	// Defines the type of a general activity
	using ActivityFunction		= std::function<void (std::vector<T>&)>;


private:
	// Holds the activity submitted by the user
	std::vector<std::pair<ActivityFunction, std::function<CommunicationProtocol (std::vector<T>&)>>> activitiesFunctions;
	Barrier startBarrier;
	Barrier compPhaseBarrier;
	Barrier commPhaseBarrier;

	// Function executed by a generic WorkerThread
	void workerFunction (int index, std::vector<LockableVector<T>> &dataVectors);


public:
	Superstep ();
	~Superstep ();

	int addActivity (ActivityFunction fun, CommunicationProtocol protocol);
	int addActivity (ActivityFunction fun, std::function<CommunicationProtocol (std::vector<T>&)> protocol);

	/* Method to execute this current superstep. It requries a vector of workers on which the activity will be run,
	 * a vector containing ConcurentQueues which hold input elements for that worker and a vector in which will be
	 * savede output elements
	 */
	int runStep (std::vector<WorkerThread> &workers,
					std::vector<LockableVector<T>> &dataVectors);

	int getActivitiesNumber ();
};




/* ==========  ==========  ==========  ==========  ========== */
/* ==========  ==========  ==========  ==========  ========== */




template<typename T>
Superstep<T>::Superstep () : startBarrier (0), compPhaseBarrier(0), commPhaseBarrier(0) {}



template<typename T>
Superstep<T>::~Superstep () {}




template<typename T>
int Superstep<T>::addActivity (ActivityFunction fun, CommunicationProtocol protocol) {
	// TODO
	auto protoFun	= std::function<Superstep<T>::CommunicationProtocol (std::vector<int> &)> ([protocol] (std::vector<int> els) {
							return protocol;
						});
	auto element	= std::make_pair<ActivityFunction, std::function<CommunicationProtocol (std::vector<T>&)>>
						(std::move(fun), std::move(protoFun));
	activitiesFunctions.push_back (std::move(element));

	return activitiesFunctions.size()-1;
}




template<typename T>
int Superstep<T>::addActivity (ActivityFunction fun, std::function<CommunicationProtocol (std::vector<T>&)> protocol) {
	// TODO
	auto element	= std::make_pair<ActivityFunction, std::function<CommunicationProtocol (std::vector<T>&)>>
						(std::move(fun), std::move(protocol));
	activitiesFunctions.push_back (std::move(element));
	
	return activitiesFunctions.size()-1;
}




template<typename T>
int Superstep<T>::runStep (std::vector<WorkerThread> &workers,
							std::vector<LockableVector<T>> &dataVectors) {

	if (workers.size () < activitiesFunctions.size()) {
		throw std::runtime_error ("Mismatching between number of workers and number of activities! (#workers < #activities)");
	}

	// Resetting bariers
	commPhaseBarrier.reset (activitiesFunctions.size());
	compPhaseBarrier.reset (activitiesFunctions.size());
	startBarrier.reset (1);


	// Setting up worker threads
	std::cout << "size: " << activitiesFunctions.size() << std::endl;;
	for (size_t i=0; i<activitiesFunctions.size(); i++) {

		// Bind packaged task to arguments
		auto lambdaFunction	= [&] (int index, std::vector<LockableVector<T>> &inputItems) {
			workerFunction (index, inputItems);
		};
		auto boundedFunction	= std::bind (lambdaFunction, i, std::ref(dataVectors));
		auto packagedTask	= std::packaged_task<void()> (boundedFunction);

		// Assign created packaged_task to a worker
		workers[i].assignActivity (std::move(packagedTask));
	}

	// Starting workers and waiting for their completion
	startBarrier.decreaseBarrier ();
	commPhaseBarrier.waitForFinish ();

	return -1;
}




template<typename T>
void Superstep<T>::workerFunction (int index, std::vector<LockableVector<T>> &inputItems) {
	startBarrier.waitForFinish ();

	// ============================
	// Performing computation phase
	activitiesFunctions[index].first (inputItems[index].getVector());
	compPhaseBarrier.decreaseBarrier ();

	std::cout << "End of computation phase\n";

	// ==============================
	// Performing communication phase
	// TODO
	CommunicationProtocol protocol	= activitiesFunctions[index].second (inputItems[index].getVector());
	/*std::this_thread::sleep_for (std::chrono::milliseconds (1000));		// Simulating communication phase*/
	commPhaseBarrier.decreaseBarrier ();
	std::cout << "Exiting from workerFunction\n";
}


#endif // SUPERSTEP_HPP