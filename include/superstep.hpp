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

#include <list>
#include <algorithm>
#include <numeric>
#include <iostream>		// REMOVE ME AFTER TESTS



template<typename T>
class Superstep {
public:
	/* Describes the data-type necessary to perform the comunication phase:
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
	void workerFunction (int index, std::vector<T> &inputVectors, std::vector<LockableVector<T>> &outputVectors);


public:
	Superstep ();
	~Superstep ();

	int addActivity (ActivityFunction fun, CommunicationProtocol protocol);
	int addActivity (ActivityFunction fun, std::function<CommunicationProtocol (std::vector<T>&)> protocol);

	/* Method to execute this current superstep. It requries a vector of workers on which the activity will be run,
	 * a vector containing vector which hold input elements for that worker and a LockableVector on which will be
	 * saved output elements
	 */
	int runStep (std::vector<WorkerThread> &workers,
					std::vector<std::vector<T>> &inputVectors,
					std::vector<LockableVector<T>> &outputVectors);

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
	auto element	= std::make_pair<ActivityFunction, std::function<CommunicationProtocol (std::vector<T>&)>>
						(std::move(fun), std::move(protocol));
	activitiesFunctions.push_back (std::move(element));
	
	return activitiesFunctions.size()-1;
}




template<typename T>
int Superstep<T>::runStep (std::vector<WorkerThread> &workers,
							std::vector<std::vector<T>>	&inputVectors,
							std::vector<LockableVector<T>> &outputVectors) {

	if (workers.size () < activitiesFunctions.size()) {
		throw std::runtime_error ("Mismatching between number of workers and number of activities! (#workers < #activities)");
	}

	// Resetting bariers
	commPhaseBarrier.reset (activitiesFunctions.size());
	compPhaseBarrier.reset (activitiesFunctions.size());
	startBarrier.reset (1);


	// Setting up worker threads
	for (size_t i=0; i<activitiesFunctions.size(); i++) {

		// Bind packaged task to arguments
		auto lambdaFunction	= [&] (int index, std::vector<std::vector<T>> &inputItems, std::vector<LockableVector<T>> &outputItems) {
			workerFunction (index, inputItems[index], outputItems);
		};
		auto boundedFunction	= std::bind (lambdaFunction, i, std::ref(inputVectors), std::ref(outputVectors));
		auto packagedTask		= std::packaged_task<void()> (boundedFunction);

		// Assign created packaged_task to a worker
		workers[i].assignActivity (std::move(packagedTask));
	}

	// Starting workers and waiting for their completion
	startBarrier.decreaseBarrier ();
	commPhaseBarrier.waitForFinish ();

	return -1;
}




template<typename T>
void Superstep<T>::workerFunction (int index, std::vector<T> &inputItems, std::vector<LockableVector<T>> &outputItems) {
	startBarrier.waitForFinish ();

	
	// ============================
	// Performing computation phase
	activitiesFunctions[index].first (inputItems);
	compPhaseBarrier.decreaseBarrier ();


	// ==============================
	// Performing communication phase
	CommunicationProtocol protocol	= activitiesFunctions[index].second (inputItems);
	std::list<int> remainingActivities (activitiesFunctions.size());
	int idx=0;
	std::iota (std::begin(remainingActivities), std::end(remainingActivities), idx++);


	while (remainingActivities.size() > 0) {
		std::list<int> &ra	= remainingActivities;
		for (auto it=ra.begin(); it!=ra.end(); it++) {

			if (protocol[*it].size() == 0) {
				remainingActivities.erase (it++);
			}
			else {
				try {
					std::unique_ptr<LockedVector<T>> targetV	= outputItems[*it].tryLockAndGet();
					auto &targetProtocol						= protocol[*it];
					// Inserting data at the end of target vector
					for (auto i : targetProtocol)
						targetV->data.insert (targetV->data.end(), std::move(inputItems[i]));

					remainingActivities.erase (it++);
				} catch (std::logic_error &e) {
					std::cout << "Cannot lock vector of i-th activity\n";
				}
			}
		}
	}

	commPhaseBarrier.decreaseBarrier ();
}


#endif // SUPERSTEP_HPP