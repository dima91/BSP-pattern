/**
 * \file superstep.hpp
 * \brief Header file for Superstep class
 * \author Luca Di Mauro
 */


#ifndef SUPERSTEP_HPP
#define SUPERSTEP_HPP

#include <workerThread.hpp>
#include <barrier.hpp>



class Superstep {
private:


public:
	Superstep ();
	~Superstep ();

	//void addActivity ();
	//std::unique_ptr<Barrier> runStep (std::vector<std::thread> &workers);
	//int getActivitiesNumber ();
};


#endif // SUPERSTEP_HPP