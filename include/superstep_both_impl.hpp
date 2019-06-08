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
#include <uTimer.hpp>

#include <list>
#include <numeric>
#include <algorithm>
#include <mutex>

//#define MAP_OF_SEQ_MODEL		// TODO REMOVE ME

#ifdef MAP_OF_SEQ_MODEL

std::mutex outputMutex;

#define PRINT(msg) {\
	std::unique_lock<std::mutex> lock (outputMutex);\
	std::cout << msg << std::endl;\
}




template<typename T>
class Superstep {
public:
	/* Describes the data-type necessary to perform the comunication phase:
	 * The i-th position of the vector contains elements which will be sent to activity i
	 * of next superstep (length of this vector should be equal to parallelism degree)
	 */
	using CommunicationProtocols	= std::vector<std::vector<T>>;

	// Defines the type of a general activity
	using ActivityFunction			= std::function<void (int, int, int, int, std::vector<T>&)>;

	/* Type of function called at the end of superstep to decide whether contiue execution or not
	 * Arguments:
	 * 	index			The index of current superstep
	 * 	outputVectors	(LockableVectors resulting at the end of communication phase
	 * Returned values:
	 *	-1		Termination reached
	 *  -2		Next superstep
	 *	n>=0	Termination not reached. 'n' is the index of the next superstep
	 */
	using AtExitFunction		= std::function<int (std::vector<LockableVector<T>>&)>;
	const int NEXT_STEP_FLAG	= -2;			// Going to next step during computation
	const int EOC_FLAG			= -1;			// End of computation


private:
	// Holds the activity submitted by the user
	std::vector<std::pair<ActivityFunction, std::function<CommunicationProtocols (int, int, int, int,
																					std::vector<T>&)>>> activitiesFunctions;
	Barrier startBarrier;
	Barrier compPhaseBarrier;
	Barrier commPhaseBarrier;
	std::mutex outputVectorsMutex;
	AtExitFunction atExitF;

	// Function executed by a generic WorkerThread
	void workerFunction (int index, std::vector<T> &inputVector, std::vector<LockableVector<T>> &outputVectors);

	void subWorkerFunction (int idx, int subWIdx, int start, int end,
								std::vector<T> &inputVector, std::vector<LockableVector<T>> &outputItems);

	static std::atomic_int nextVectorToLock;


public:
	Superstep ();
	~Superstep ();

	int addActivity (ActivityFunction fun, CommunicationProtocols protocol);
	int addActivity (ActivityFunction fun, std::function<CommunicationProtocols (int, int, int, int, std::vector<T>&)> protocol);

	/* Method to execute this current superstep. It requries a vector of workers on which the activity will be run,
	 * a vector containing vector which hold input elements for that worker and a LockableVector on which will be
	 * saved output elements
	 */
	int runStep (int q,
					std::vector<WorkerThread> &workers,
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
		return NEXT_STEP_FLAG;
	});
}




template<typename T>
Superstep<T>::~Superstep () {}




template<typename T>
int Superstep<T>::addActivity (ActivityFunction fun, CommunicationProtocols protocols) {
	auto protoFun	= std::function<Superstep<T>::CommunicationProtocols (std::vector<int> &)> ([protocols] (std::vector<int> els) {
							// FIXME In this case 'protocols' array contains the indexes of array --> convert it into elements
							return protocols;
						});
	auto element	= std::make_pair<ActivityFunction, std::function<CommunicationProtocols (std::vector<T>&)>>
						(std::move(fun), std::move(protoFun));

	activitiesFunctions.emplace_back (std::move(element));

	return activitiesFunctions.size()-1;
}




template<typename T>
int Superstep<T>::addActivity (ActivityFunction fun, std::function<CommunicationProtocols (int, int, int, int,
																							std::vector<T>&)> protocol) {
	auto element	= std::make_pair<ActivityFunction, std::function<CommunicationProtocols (int, int, int,  int, std::vector<T>&)>>
						(std::move(fun), std::move(protocol));
	activitiesFunctions.emplace_back (std::move(element));
	
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
int Superstep<T>::runStep (int q,
							std::vector<WorkerThread> &workers,
							std::vector<std::vector<T>>	&inputVectors,
							std::vector<LockableVector<T>> &outputVectors) {

	// TOOD Add checks on q
	if (workers.size () < activitiesFunctions.size()*q) {
		throw std::runtime_error ("Mismatching between number of workers and number of activities! (#workers < #activities*q)");
	}
	if (inputVectors.size() < activitiesFunctions.size())
		throw std::runtime_error ("Mismatching between number of activities and number of input queues! (#activities > #queues)");

	//PRINT ("Starting superstep with p: " << activitiesFunctions.size() << " and q: " << q);

	// Resetting bariers
	commPhaseBarrier.reset (activitiesFunctions.size()*q);
	compPhaseBarrier.reset (activitiesFunctions.size()*q);
	startBarrier.reset (1);


	// Setting up worker threads
	for (size_t i=0; i<activitiesFunctions.size(); i++) {
		int k	= inputVectors[i].size()/q;
		for (int j=0; j<q; j++) {
			// Bind packaged task to arguments
			auto lambdaFunction	= [&] (int index, int subwWIdx, int start, int end, std::vector<T> &inputItems, std::vector<LockableVector<T>> &outputItems) {
				subWorkerFunction (index, subwWIdx, start, end, inputItems, outputItems);
			};
			auto startIdx			= (j*k);
			auto endIdx				= ((j+1)*k);
			auto boundedFunction	= std::bind (lambdaFunction, i, j, startIdx, endIdx, std::ref(inputVectors[i]), std::ref(outputVectors));
			auto packagedTask		= std::packaged_task<void()> (boundedFunction);



			// Assign created packaged_task to a worker
			workers[(i*q)+j].assignActivity (std::move(packagedTask));
		}
	}

	// Starting workers and waiting for their completion
	startBarrier.decreaseBarrier ();
	commPhaseBarrier.waitForFinish ();
	
	return atExitF (outputVectors);
}




template<typename T>
void Superstep<T>::subWorkerFunction (int idx, int subWIdx, int start, int end, std::vector<T> &inputVector,
										std::vector<LockableVector<T>> &outputItems) {
	startBarrier.waitForFinish ();
	
	// ============================
	// Performing computation phase
	//std::cout << "IVLen " << inputVector.size() << std::endl;
	activitiesFunctions[subWIdx].first (idx, subWIdx, start, end, inputVector);
	compPhaseBarrier.decreaseBarrier ();

	// ==============================
	// Performing communication phase
	// Computing vector which defines the communication phase
	CommunicationProtocols protocols	= activitiesFunctions[idx].second (idx, subWIdx, start, end, std::ref(inputVector));

	if (protocols.size() != 0) {
		// Checking if a resize of outputvectors is needed
		{
			std::unique_lock<std::mutex> lock (outputVectorsMutex);
			
			if (protocols.size() > outputItems.size()) {
				outputItems.resize (protocols.size ());
			}
		}

		std::list<int> remainingActivities (protocols.size());
		int idx	= 0;
		std::iota (std::begin(remainingActivities), std::end(remainingActivities), idx++);


		// TODO Try to drop the offset!
		int vectorOffset	= (nextVectorToLock++) % remainingActivities.size();
		while (remainingActivities.size() > 0) {
			auto it	= std::begin (remainingActivities);
			std::advance (it, vectorOffset);

			while (it!=remainingActivities.end()) {

				if (protocols[*it].size() == 0) {
					remainingActivities.erase (it++);
				}
				else {
					try {
						// TODO Try to lock without tryLockAndGet
						std::shared_ptr<LockedVector<T>> targetV	= outputItems[*it].tryLockAndGet();
						auto &targetProtocol						= protocols[*it];
						
						// Inserting data at the end of target vector
						for (auto i : targetProtocol) {
							targetV->data.emplace_back (i);
						}

						remainingActivities.erase (it++);
					} catch (std::logic_error &e) {
						it++;
					}
				}
			}
			vectorOffset	= 0;
		}
	}

		commPhaseBarrier.decreaseBarrier ();
}


#else

extern std::mutex outputMutex;

#define PRINT(msg) {\
	std::unique_lock<std::mutex> lock (outputMutex);\
	std::cout << msg << std::endl;\
}


template<typename T>
class Superstep {
public:
	/* Describes the data-type necessary to perform the comunication phase:
	 * The i-th position of the vector contains elements which will be sent to activity i
	 * of next superstep (length of this vector should be equal to parallelism degree)
	 */
	using CommunicationProtocols	= std::vector<std::vector<T>>;

	// Defines the type of a general activity
	using ActivityFunction			= std::function<void (int, std::vector<T>&)>;

	/* Type of function called at the end of superstep to decide whether contiue execution or not
	 * Arguments:
	 * 	index			The index of current superstep
	 * 	outputVectors	(LockableVectors resulting at the end of communication phase
	 * Returned values:
	 *	-1		Termination reached
	 *  -2		Next superstep
	 *	n>=0	Termination not reached. 'n' is the index of the next superstep
	 */
	using AtExitFunction		= std::function<int (std::vector<LockableVector<T>>&)>;
	const int NEXT_STEP_FLAG	= -2;			// Going to next step during computation
	const int EOC_FLAG			= -1;			// End of computation


private:
	// Holds the activity submitted by the user
	std::vector<std::pair<ActivityFunction, std::function<CommunicationProtocols (int, std::vector<T>&)>>> activitiesFunctions;
	Barrier startBarrier;
	Barrier compPhaseBarrier;
	Barrier commPhaseBarrier;
	std::mutex outputVectorsMutex;
	AtExitFunction atExitF;

	// Function executed by a generic WorkerThread
	void workerFunction (int index, std::vector<T> &inputVector, std::vector<LockableVector<T>> &outputVectors);

	static std::atomic_int nextVectorToLock;


public:
	Superstep ();
	~Superstep ();

	int addActivity (ActivityFunction fun, CommunicationProtocols protocol);
	int addActivity (ActivityFunction fun, std::function<CommunicationProtocols (int, std::vector<T>&)> protocol);

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
		return NEXT_STEP_FLAG;
	});
}




template<typename T>
Superstep<T>::~Superstep () {}




template<typename T>
int Superstep<T>::addActivity (ActivityFunction fun, CommunicationProtocols protocols) {
	auto protoFun	= std::function<Superstep<T>::CommunicationProtocols (std::vector<int> &)> ([protocols] (std::vector<int> els) {
							// FIXME In this case 'protocols' array contains the indexes of array --> convert it into elements
							return protocols;
						});
	auto element	= std::make_pair<ActivityFunction, std::function<CommunicationProtocols (std::vector<T>&)>>
						(std::move(fun), std::move(protoFun));

	activitiesFunctions.emplace_back (std::move(element));

	return activitiesFunctions.size()-1;
}




template<typename T>
int Superstep<T>::addActivity (ActivityFunction fun, std::function<CommunicationProtocols (int activityIndex, std::vector<T>&)> protocol) {
	auto element	= std::make_pair<ActivityFunction, std::function<CommunicationProtocols (int activityIndex, std::vector<T>&)>>
						(std::move(fun), std::move(protocol));
	activitiesFunctions.emplace_back (std::move(element));
	
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
	if (inputVectors.size() < activitiesFunctions.size())
		throw std::runtime_error ("Mismatching between number of activities and number of input queues! (#activities > #queues)");

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

	// Starting workers and waiting for their completion
	startBarrier.decreaseBarrier ();
	commPhaseBarrier.waitForFinish ();
	
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
	// Computing vector which defines the communication phase
	CommunicationProtocols protocols	= activitiesFunctions[index].second (index, std::ref(inputItems));

	if (protocols.size() != 0) {
		// Checking if a resize of outputvectors is needed
		{
			std::unique_lock<std::mutex> lock (outputVectorsMutex);
			
			if (protocols.size() > outputItems.size()) {
				outputItems.resize (protocols.size ());
			}
		}

		std::list<int> remainingActivities (protocols.size());
		int idx	= 0;
		std::iota (std::begin(remainingActivities), std::end(remainingActivities), idx++);


		// TODO Try to drop the offset!
		int vectorOffset	= (nextVectorToLock++) % remainingActivities.size();
		while (remainingActivities.size() > 0) {
			auto it	= std::begin (remainingActivities);
			std::advance (it, vectorOffset);

			while (it!=remainingActivities.end()) {

				if (protocols[*it].size() == 0) {
					remainingActivities.erase (it++);
				}
				else {
					try {
						// TODO Try to lock without tryLockAndGet
						std::shared_ptr<LockedVector<T>> targetV	= outputItems[*it].tryLockAndGet();
						auto &targetProtocol						= protocols[*it];
						
						// Inserting data at the end of target vector
						for (auto i : targetProtocol) {
							targetV->data.emplace_back (i);
						}

						remainingActivities.erase (it++);
					} catch (std::logic_error &e) {
						it++;
					}
				}
			}
			vectorOffset	= 0;
		}
	}

	outputItems.shrink_to_fit ();

	commPhaseBarrier.decreaseBarrier ();
}


#endif


#endif // SUPERSTEP_HPP