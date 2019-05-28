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
#include <numeric>
#include <algorithm>
//#include <iostream> // FIXME REMOVE ME AFTER TESTS
#include <future> // FIXME REMOVE ME AFTER TESTS



template<typename T>
class Superstep {
public:
	/* Describes the data-type necessary to perform the comunication phase:
	 * The i-th position of the vector contains elements which will be sent to activity i
	 * of next superstep (length of this vector should be equal to parallelism degree)
	 */
	using CommunicationProtocol	= std::vector<std::vector<T>>;

	// Defines the type of a general activity
	using ActivityFunction		= std::function<void (int, std::vector<T>&)>;

	/* Type of function called at the end of superstep to decide whether contiue execution or not
	 * Arguments:
	 * 	index			The index of current superstep
	 * 	outputVectors	(LockableVectors resulting at the end of communication phase
	 * Returned values:
	 *	-1		Termination reached
	 *  -2		Next superstep
	 *	n>=0	Termination not reached. 'n' is the index of the next superstep
	 */
	using AtExitFunction	= std::function<int (std::vector<LockableVector<T>>&)>;


private:
	// Holds the activity submitted by the user
	std::vector<std::pair<ActivityFunction, std::function<CommunicationProtocol (int, std::vector<T>&)>>> activitiesFunctions;
	//const int currentSSIndex;
	Barrier startBarrier;
	Barrier compPhaseBarrier;
	Barrier commPhaseBarrier;
	AtExitFunction atExitF;

	// Function executed by a generic WorkerThread
	void workerFunction (int index, std::vector<T> &inputVector, std::vector<LockableVector<T>> &outputVectors);

	static std::atomic_int nextVectorToLock;


public:
	Superstep ();
	~Superstep ();

	int addActivity (ActivityFunction fun, CommunicationProtocol protocol);
	int addActivity (ActivityFunction fun, std::function<CommunicationProtocol (int, std::vector<T>&)> protocol);

	/* Method to execute this current superstep. It requries a vector of workers on which the activity will be run,
	 * a vector containing vector which hold input elements for that worker and a LockableVector on which will be
	 * saved output elements
	 */
	int runStep (std::vector<WorkerThread> &workers,
					std::vector<std::vector<T>> &inputVectors,
					std::vector<LockableVector<T>> &outputVectors);

	int getActivitiesNumber ();
	void setAtExitFunction (AtExitFunction atExit);
};


template<typename T>
std::atomic_int Superstep<T>::nextVectorToLock;




/* ==========  ==========  ==========  ==========  ========== */
/* ==========  ==========  ==========  ==========  ========== */




template<typename T>
Superstep<T>::Superstep () : startBarrier (0), compPhaseBarrier(0), commPhaseBarrier(0) {
	atExitF	= AtExitFunction ([&] (std::vector<LockableVector<T>> &outputItems) {
		return -2;
	});
}




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
int Superstep<T>::addActivity (ActivityFunction fun, std::function<CommunicationProtocol (int activityIndex, std::vector<T>&)> protocol) {
	auto element	= std::make_pair<ActivityFunction, std::function<CommunicationProtocol (int activityIndex, std::vector<T>&)>>
						(std::move(fun), std::move(protocol));
	activitiesFunctions.push_back (std::move(element));
	
	return activitiesFunctions.size()-1;
}




template<typename T>
int Superstep<T>::getActivitiesNumber () {
	return activitiesFunctions.size();
}




template<typename T>
void Superstep<T>::setAtExitFunction (AtExitFunction atExitFun) {
	atExitF	= atExitFun;
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
		auto lambdaFunction	= [&] (int index, std::vector<T> &inputItems, std::vector<LockableVector<T>> &outputItems) {
			workerFunction (index, inputItems, outputItems);
		};
		auto boundedFunction	= std::bind (lambdaFunction, i, std::ref(inputVectors[i]), std::ref(outputVectors));
		auto packagedTask		= std::packaged_task<void()> (boundedFunction);

		// Assign created packaged_task to a worker
		workers[i].assignActivity (std::move(packagedTask));
	}

	/*auto barrierPtr	= &commPhaseBarrier;
	auto asyncWork	= std::async (std::launch::async, [barrierPtr] () {
		while (barrierPtr->getRemainingEntities())  {
			std::this_thread::sleep_for (std::chrono::seconds (2));
			std::cout << "RW: " << barrierPtr->getRemainingEntities() << std::endl;
		}
	});*/

	// Starting workers and waiting for their completion
	startBarrier.decreaseBarrier ();
	commPhaseBarrier.waitForFinish ();
	//asyncWork.get();
	
	return atExitF (outputVectors);
}




template<typename T>
void Superstep<T>::workerFunction (int index, std::vector<T> &inputItems, std::vector<LockableVector<T>> &outputItems) {
	startBarrier.waitForFinish ();

	
	// ============================
	// Performing computation phase
	activitiesFunctions[index].first (index, inputItems);
	compPhaseBarrier.decreaseBarrier ();


	// ==============================
	// Performing communication phase
	CommunicationProtocol protocol	= activitiesFunctions[index].second (index, std::ref(inputItems));
	std::list<int> remainingActivities (activitiesFunctions.size());
	int idx	= 0;
	std::iota (std::begin(remainingActivities), std::end(remainingActivities), idx++);


	int vectorOffset	= (nextVectorToLock++) % remainingActivities.size();
	while (remainingActivities.size() > 0) {
		std::list<int> &ra	= remainingActivities;
		auto it	= std::begin(ra);
		std::advance (it, vectorOffset);
		while (it!=ra.end()) {

			if (protocol[*it].size() == 0) {
				remainingActivities.erase (it++);
			}
			else {
				try {
					std::shared_ptr<LockedVector<T>> targetV	= outputItems[*it].tryLockAndGet();
					auto &targetProtocol						= protocol[*it];
					// Inserting data at the end of target vector
					for (auto i : targetProtocol) {
						targetV->data.insert (targetV->data.end(), std::move(i));
					}

					remainingActivities.erase (it++);
				} catch (std::logic_error &e) {
					it++;
				}
			}
		}
		vectorOffset	= 0;
	}

	commPhaseBarrier.decreaseBarrier ();
}


#endif // SUPERSTEP_HPP