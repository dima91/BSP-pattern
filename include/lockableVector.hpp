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

// Forward class declaration
template<typename T>
class LockableVector;



template<typename T>
class LockedVector {
private :
	LockableVector<T>& parent;
public :
	std::vector<T>& data;
	LockedVector (std::vector<T>& origVect, LockableVector<T> *locker) : parent (*locker), data (origVect) {}
	~LockedVector () {
		parent.releaseVector ();
	}
};




template <typename T>
class LockableVector {
private:
	friend class LockedVector<T>;
	std::vector<T> dataVector;
	std::mutex vectorMutex;
	std::condition_variable vectorCV;

	void releaseVector () {
		vectorMutex.unlock ();
	}

	
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


	std::unique_ptr<LockedVector<T>> lockAndGet () {
		vectorMutex.lock ();
		return std::unique_ptr<LockedVector<T>> (new LockedVector<T> (dataVector, this));
	}
};


#endif // LOCAKBLE_VECTOR_HPP