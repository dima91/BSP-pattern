/**
 * \file bsp.hpp
 * \brief Header file for Bsp class
 * \author Luca Di Mauro
 */


#ifndef BSP_HPP
#define BSP_HPP

#define PRINT_VECTOR(label, v) {\
		std::cout << label << "\n";\
		for (size_t i=0; i<v.size(); i++) {\
			std::cout << i << " >  ";\
			for (auto el : v[i])\
				std::cout << el << "\t";\
			std::cout << std::endl;\
		}\
}

#include <workerThread.hpp>
#include <superstep.hpp>

#include <vector>



template <typename T>
class BSP {
public:
	using SuperstepPointer	= std::shared_ptr<Superstep<T>>;	// ?
	const int NextStepFlag	= -2;								// Going to next step during computation
	const int EOCFlag		= -1;								// End of computation


private:
	std::vector<WorkerThread> workers;
	std::vector<SuperstepPointer> supersteps;

	void setupWorkers ();
	void swapVectors (std::vector<std::vector<T>> &a, std::vector<LockableVector<T>> &b);


public:
	BSP ();
	~BSP ();


	int addSuperstep (SuperstepPointer step);

	void runAndWait (std::vector<std::vector<T>> &input, std::vector<std::vector<T>> &ouput);
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
void BSP<T>::setupWorkers () {
	int maxActivities	= 0;
	for (size_t i=0; i<supersteps.size(); i++) {
		int tmpNum		= supersteps[i]->getActivitiesNumber();
		maxActivities	= (maxActivities >= tmpNum) ? maxActivities : tmpNum;
	};

	workers	= std::vector<WorkerThread> (maxActivities);
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
void BSP<T>::runAndWait (std::vector<std::vector<T>> &input, std::vector<std::vector<T>> &output) {
	int retVal		= 0;
	int nextStep	= 0;
	std::vector<LockableVector<T>> lockableVectors (output.size());

	setupWorkers ();

	swapVectors (output, lockableVectors);

	while (retVal != EOCFlag && nextStep < (int) supersteps.size()) {

		retVal	= supersteps[nextStep]->runStep (workers, input, lockableVectors);

		if (retVal == NextStepFlag) {
			nextStep++;
			swapVectors (input, lockableVectors);
		}
		else if (retVal == EOCFlag) {
			// TODO
		}
		else {
			nextStep	= retVal;
			swapVectors (input, lockableVectors);
		}
	}


	output	= input;
}


#endif // BSP_HPP