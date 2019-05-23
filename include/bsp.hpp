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



template <typename T>
class BSP {
public:
	using SuperstepPointer	= std::shared_ptr<Superstep<T>>;


private:
	std::vector<WorkerThread> workers;
	std::vector<SuperstepPointer> supersteps;

	void setupWorkers ();


public:
	BSP ();
	~BSP ();


	void addSuperstep (SuperstepPointer step);
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
	// TOOD
}




template<typename T>
void BSP<T>::addSuperstep (SuperstepPointer step) {
	// TODO
	supersteps.emplace_back (step);
}




template<typename T>
void BSP<T>::runAndWait (std::vector<std::vector<T>> &input, std::vector<std::vector<T>> &ouput) {
	// TODO
}


#endif // BSP_HPP