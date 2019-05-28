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
#include <iostream>



template <typename T>
class BSP {
public:
	using SuperstepPointer	= std::shared_ptr<Superstep<T>>;	// ?
	const int NextStepFlag	= -2;								// Going to next step during computation
	const int EOCFlag		= -1;								// End of computation


private:
	std::vector<WorkerThread> workers;
	std::vector<SuperstepPointer> supersteps;

	void setupWorkers (bool setAffinity);
	void swapVectors (std::vector<std::vector<T>> &a, std::vector<LockableVector<T>> &b);


public:
	BSP ();
	~BSP ();


	int addSuperstep (SuperstepPointer step);

	void runAndWait (std::vector<std::vector<T>> &input, std::vector<std::vector<T>> &ouput, bool setAffinity);
};




/* ==========  ==========  ==========  ==========  ========== */
/* ==========  ==========  ==========  ==========  ========== */




template<typename T>
BSP<T>::BSP () {
	// TOOD
}




template<typename T>
BSP<T>::~BSP () {
	// TODO
}




template<typename T>
int BSP<T>::addSuperstep (SuperstepPointer step) {
	supersteps.emplace_back (step);
	return supersteps.size()-1;
}




template<typename T>
void BSP<T>::setupWorkers (bool setAffinity) {
	int maxActivities	= 0;
	for (size_t i=0; i<supersteps.size(); i++) {
		int tmpNum		= supersteps[i]->getActivitiesNumber();
		maxActivities	= (maxActivities >= tmpNum) ? maxActivities : tmpNum;
	};

	workers	= std::vector<WorkerThread> (maxActivities);

	if (setAffinity) {
		unsigned int cpusNum	= std::thread::hardware_concurrency();
		
		if ((int) cpusNum < maxActivities)
			std::cout << "WARNING!\tTrying to set affinity for workers which number is greater than number of CPUs!\n";
		
		int idx	= 0;
		std::for_each (workers.begin(), workers.end(), [&idx, cpusNum] (WorkerThread &w) {
			w.setAffinity ((idx++) % cpusNum);
		});
	}
}




template<typename T>
void BSP<T>::swapVectors (std::vector<std::vector<T>> &a, std::vector<LockableVector<T>> &b) {
	// FIXME Considerare il caso in cui i due array abbiamo dimensioni diverse
	for (size_t i=0; i< b.size(); i++) {
		b[i].swap (a[i]);
	}

	for (size_t i=0; i< b.size(); i++) {
		b[i].getVector().clear ();
	}
}




template<typename T>
void BSP<T>::runAndWait (std::vector<std::vector<T>> &input, std::vector<std::vector<T>> &output, bool setAffinity) {
	int retVal		= 0;
	int nextStep	= 0;
	std::vector<LockableVector<T>> lockableVectors (output.size());

	setupWorkers (setAffinity);

	//std::this_thread::sleep_for (std::chrono::seconds (4));

	swapVectors (output, lockableVectors);

	while (retVal != EOCFlag && nextStep < (int) supersteps.size()) {

		{
			UTimer timer ("Superstep " + std::to_string (nextStep));
			std::cout << "Running superstep  " << nextStep << std::endl;
			retVal	= supersteps[nextStep]->runStep (workers, input, lockableVectors);
		}

		{
			UTimer timer ("Swap vectors");
			std::cout << "Swapping vectors" << std::endl;
			if (retVal == NextStepFlag) {
				nextStep++;
				swapVectors (input, lockableVectors);
			}
			else if (retVal == EOCFlag) {
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


#endif // BSP_HPP