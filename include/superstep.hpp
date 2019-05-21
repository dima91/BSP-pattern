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
	ActivityFunction activityFunction;	// Holds the activity submitted by the user
	int parallelismDegree;				// Holds the number of parallel activities for this superstep
	CommunicationProtocol commProto;	// Definses the communication protocol executed at the end of computation phase
	Barrier workersBarrier;				// Allows to activities to wait for other activities' end of computation/communication
	Barrier startBarrier;				// TODO


public:
	Superstep (ActivityFunction fun, int parDegree, CommunicationProtocol proto);
	~Superstep ();

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
Superstep<T>::Superstep (ActivityFunction fun, int parDegree, CommunicationProtocol proto) :
workersBarrier(0), startBarrier (0) {
	activityFunction	= fun;
	parallelismDegree	= parDegree;
	commProto			= proto;
}



template<typename T>
Superstep<T>::~Superstep () {
	// TODO
}




template<typename T>
int Superstep<T>::runStep (std::vector<WorkerThread> &workers,
							std::vector<LockableVector<T>> &dataVectors) {

	// TODO Defining worker function
	auto computationFunction	= std::function<void (std::vector<T>&)> ([&] (std::vector<T>& inputItems) {
		startBarrier.waitForFinish ();
		std::cout << "Running activityFunction\n";
		activityFunction (inputItems);
		workersBarrier.decreaseBarrier ();
		//std::cout << "exiting from computationFunction  " << workersBarrier.getRemainingEntities() << std::endl;
	});

	// TODO CommunicationFunction
	/*auto communicationFunction	= std::function<void (std::vector<LockableVector<T>>&)>
									([&] (std::vector<LockableVector<T>>& inputItems) {
		std::this_thread::sleep_for (std::chrono::milliseconds (1000));
		barrier.decreaseBarrier ();
		std::cout << "exiting..\n";
	});*/

	// ============================
	// Setting up computation phase
	if (workers.size () < (size_t) parallelismDegree) {
		throw std::runtime_error ("Mismatching number of workers vector and parallelism degree! (#workers < parallelism degree)");
	}

	workersBarrier.reset (parallelismDegree);
	startBarrier.reset (1);	
	for (int i=0; i<parallelismDegree; i++) {
		// Bind packaged task to arguments
		auto boundFunction	= std::bind (computationFunction, dataVectors[i].getVector ());
		auto packagedTask	= std::packaged_task<void()> (boundFunction);

		// Assign created packaged_task to a worker
		workers[i].assignActivity (std::move(packagedTask));
	}


	// ================================
	// Waiting for workers completion
	std::cout << "Decreasing startBarrier and waiting for workers' end..\n";
	startBarrier.decreaseBarrier ();
	workersBarrier.waitForFinish ();
	std::cout << "Workers finished\n";


	// ==============================
	// Setting up communication phase
	/*startBarrier.reset (1);
	workersBarrier.reset (parallelismDegree);*/

	return -1;
}


#endif // SUPERSTEP_HPP