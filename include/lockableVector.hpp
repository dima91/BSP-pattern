/**
 * \file lockableVector.hpp
 * \brief Definition and implementation of LockableVector class
 * \author Luca Di Mauro
 */


#ifndef LOCAKBLE_VECTOR_HPP
#define LOCAKBLE_VECTOR_HPP

#include <vector>
#include <mutex>
#include <condition_variable>



template <typename T>
class LockableVector {
private:
	std::vector<T> dataVector;
	std::mutex vectorMutex;
	std::condition_variable vectorCV;

	
public:
	LockableVector (std::vector<T> &externalVector) : dataVector(externalVector) {
	}

	LockableVector () {
	}

	~LockableVector () {
		// TODO
	}


	bool isEmpty () {
		std::unique_lock<std::mutex> lock (vectorMutex);
		return dataVector.empty ();
	}


	std::vector<T>& getVector () {
		return dataVector;
	}


	std::vector<T>& lockAndGet () {
		vectorMutex.lock ();
		return dataVector;
	}


	void releaseVector () {
		vectorMutex.unlock ();
	}
};


#endif // LOCAKBLE_VECTOR_HPP