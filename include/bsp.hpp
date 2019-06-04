/**
 * \file bsp.hpp
 * \brief Header file for Bsp class
 * \author Luca Di Mauro
 */


#ifndef BSP_HPP
#define BSP_HPP

#include <workerThread.hpp>
#include <superstep.hpp>
#include <uTimer.hpp>

#include <vector>

//#define MAP_OF_SEQ_MODEL		// TODO  REMOVE ME!

#ifdef MAP_OF_SEQ_MODEL



template <typename T>
class BSP {
public:
	using SuperstepPointer	= std::shared_ptr<Superstep<T>>;


private:
	std::vector<WorkerThread> workers;
	std::vector<SuperstepPointer> supersteps;

	void setupWorkers (int q, bool setAffinity);
	void swapVectors (std::vector<std::vector<T>> &a, std::vector<LockableVector<T>> &b);

	const int NEXT_STEP_FLAG	= -2;			// Going to next step during computation
	const int EOC_FLAG			= -1;			// End of computation


public:
	BSP ();
	~BSP ();


	uint addSuperstep (SuperstepPointer step);

	void runAndWait (int q, std::vector<std::vector<T>> &input, std::vector<std::vector<T>> &ouput, bool setAffinity);
};




/* ==========  ==========  ==========  ==========  ========== */
/* ==========  ==========  ==========  ==========  ========== */




template<typename T>
BSP<T>::BSP () {}




template<typename T>
BSP<T>::~BSP () {
	std::for_each (workers.begin(), workers.end(), [] (WorkerThread &w) {
		w.stopWorker ();
	});
}




template<typename T>
uint BSP<T>::addSuperstep (SuperstepPointer step) {
	supersteps.emplace_back (step);
	return supersteps.size()-1;
}




template<typename T>
void BSP<T>::setupWorkers (int q, bool setAffinity) {
	uint maxActivities	= 0;
	for (size_t i=0; i<supersteps.size(); i++) {
		uint tmpNum		= supersteps[i]->getActivitiesNumber();
		maxActivities	= (maxActivities >= tmpNum) ? maxActivities : tmpNum;
	};

	workers			= std::vector<WorkerThread> (q*maxActivities);
	uint cpusNum	= std::thread::hardware_concurrency();

	if (setAffinity) {
		uint idx	= 0;
		std::for_each (workers.begin(), workers.end(), [&idx, cpusNum] (WorkerThread &w) {
			w.setAffinity ((idx++) % cpusNum);
		});
	}
}




template<typename T>
void BSP<T>::swapVectors (std::vector<std::vector<T>> &a, std::vector<LockableVector<T>> &b) {
	int aSize	= a.size ();
	int bSize	= b.size ();

	// Checking if vectors have different sizes
	if (aSize > bSize) {
		b.resize (aSize);
	}
	else if ( aSize < bSize) {
		a.resize (bSize);
	}

	for (size_t i=0; i< b.size(); i++) {
		b[i].swap (a[i]);
	}

	for (size_t i=0; i< b.size(); i++) {
		b[i].getVector().clear ();
	}

	a.shrink_to_fit ();
	b.shrink_to_fit ();
}




template<typename T>
void BSP<T>::runAndWait (int q, std::vector<std::vector<T>> &input, std::vector<std::vector<T>> &output, bool setAffinity) {
	int retVal		= 0;
	int nextStep	= 0;
	std::vector<LockableVector<T>> lockableVectors (output.size());

	setupWorkers (q, setAffinity);

	swapVectors (output, lockableVectors);
	

	while (retVal != EOC_FLAG && nextStep < (int) supersteps.size()) {

		{
			UTimer timer ("Superstep " + std::to_string (nextStep));
			std::cout << "\n\nRunning superstep  " << nextStep << std::endl;
			retVal	= supersteps[nextStep]->runStep (q, workers, input, lockableVectors);

			// Swapping vectors
			if (retVal == NEXT_STEP_FLAG) {
				nextStep++;
				swapVectors (input, lockableVectors);
			}
			else if (retVal == EOC_FLAG) {
				swapVectors (input, lockableVectors);
			}
			else {
				nextStep	= retVal;
				swapVectors (input, lockableVectors);
			}
		}
	}

	
	std::swap (output, input);

	std::for_each (workers.begin(), workers.end(), [] (WorkerThread &w) {
		w.stopWorker ();
	});
}

#else


template <typename T>
class BSP {
public:
	using SuperstepPointer	= std::shared_ptr<Superstep<T>>;


private:
	std::vector<WorkerThread> workers;
	std::vector<SuperstepPointer> supersteps;

	void setupWorkers (bool setAffinity);
	void swapVectors (std::vector<std::vector<T>> &a, std::vector<LockableVector<T>> &b);

	const int NEXT_STEP_FLAG	= -2;			// Going to next step during computation
	const int EOC_FLAG			= -1;			// End of computation


public:
	BSP ();
	~BSP ();


	uint addSuperstep (SuperstepPointer step);

	void runAndWait (std::vector<std::vector<T>> &input, std::vector<std::vector<T>> &ouput, bool setAffinity);
};




/* ==========  ==========  ==========  ==========  ========== */
/* ==========  ==========  ==========  ==========  ========== */




template<typename T>
BSP<T>::BSP () {}




template<typename T>
BSP<T>::~BSP () {
	std::for_each (workers.begin(), workers.end(), [] (WorkerThread &w) {
		w.stopWorker ();
	});
}




template<typename T>
uint BSP<T>::addSuperstep (SuperstepPointer step) {
	supersteps.emplace_back (step);
	return supersteps.size()-1;
}




template<typename T>
void BSP<T>::setupWorkers (bool setAffinity) {
	uint maxActivities	= 0;
	for (size_t i=0; i<supersteps.size(); i++) {
		uint tmpNum		= supersteps[i]->getActivitiesNumber();
		maxActivities	= (maxActivities >= tmpNum) ? maxActivities : tmpNum;
	};

	workers			= std::vector<WorkerThread> (maxActivities);
	uint cpusNum	= std::thread::hardware_concurrency();

	if (setAffinity) {
		uint idx	= 0;
		std::for_each (workers.begin(), workers.end(), [&idx, cpusNum] (WorkerThread &w) {
			w.setAffinity ((idx++) % cpusNum);
		});
	}
}




template<typename T>
void BSP<T>::swapVectors (std::vector<std::vector<T>> &a, std::vector<LockableVector<T>> &b) {
	int aSize	= a.size ();
	int bSize	= b.size ();

	// Checking if vectors have different sizes
	if (aSize > bSize) {
		b.resize (aSize);
	}
	else if ( aSize < bSize) {
		a.resize (bSize);
	}

	for (size_t i=0; i< b.size(); i++) {
		b[i].swap (a[i]);
	}

	for (size_t i=0; i< b.size(); i++) {
		b[i].getVector().clear ();
	}

	a.shrink_to_fit ();
	b.shrink_to_fit ();
}




template<typename T>
void BSP<T>::runAndWait (std::vector<std::vector<T>> &input, std::vector<std::vector<T>> &output, bool setAffinity) {
	int retVal		= 0;
	int nextStep	= 0;
	std::vector<LockableVector<T>> lockableVectors (output.size());

	setupWorkers (setAffinity);

	swapVectors (output, lockableVectors);
	

	while (retVal != EOC_FLAG && nextStep < (int) supersteps.size()) {

		{
			UTimer timer ("Superstep " + std::to_string (nextStep));
			std::cout << "\n\nRunning superstep  " << nextStep << std::endl;
			retVal	= supersteps[nextStep]->runStep (workers, input, lockableVectors);

			// Swapping vectors
			if (retVal == NEXT_STEP_FLAG) {
				nextStep++;
				swapVectors (input, lockableVectors);
			}
			else if (retVal == EOC_FLAG) {
				swapVectors (input, lockableVectors);
			}
			else {
				nextStep	= retVal;
				swapVectors (input, lockableVectors);
			}
		}
	}

	
	std::swap (output, input);

	std::for_each (workers.begin(), workers.end(), [] (WorkerThread &w) {
		w.stopWorker ();
	});
}


#endif	// MAP_OF_SEQ_MODEL


#endif // BSP_HPP