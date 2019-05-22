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
private:
	std::vector<WorkerThread> workers;
	std::vector<Superstep<T>> supersteps;


public:
	BSP (int wNumber = 0) {
		if (wNumber < 0)
			throw std::invalid_argument ("Number of worker must be greater or equal to 0");

		workers.reserve (wNumber);
	}


	~BSP () {
	}


	//void addSuperStep (Superstep sstep);


	//void runAndWait (std::vector<std::vector<T>> input, std::vector<std::vector<T>> &ouput);
};


#endif // BSP_HPP