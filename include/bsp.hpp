/**
 * \file bsp.hpp
 * \brief Header file for Bsp class
 * \author Luca Di Mauro
 */


#ifndef BSP_HPP
#define BSP_HPP

#include <concurrentQueue.hpp>

#include <thread>
#include <vector>



template <typename T>
class Bsp {
private :
	std::vector<std::thread> workers;
	std::thread masterThread;

public :
	Bsp (int wNumber) {
	}


	~Bsp () {
	}

	//void addStep (...);
};


#endif // BSP_HPP