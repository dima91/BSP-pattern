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



/* Class which represent an already locked vector owned by related 'LockableVector' class instance.
 * In the destructor, original vector is released.
 */
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




/* Class which stores a vector of type T and allows to get it either in a shared environment using synchronization mechanisms,
 * or obtaining it without synchronization.
 */
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

	LockableVector (LockableVector &&lv) {
		dataVector	= std::vector<T> (lv.dataVector.size());
	}

	LockableVector () {
		// Do nothing
	}


	~LockableVector () {
		// Do nothing
	}


	bool isEmpty () {
		std::unique_lock<std::mutex> lock (vectorMutex);
		return dataVector.empty ();
	}


	std::vector<T>& getVector () {
		return dataVector;
	}

	void swap (std::vector<T> &targetVector) {
		std::unique_lock<std::mutex> lock (vectorMutex);
		std::swap (dataVector, targetVector);
	}


	std::shared_ptr<LockedVector<T>> lockAndGet () {
		vectorMutex.lock ();
		return std::shared_ptr<LockedVector<T>> (new LockedVector<T> (dataVector, this));
	}


	std::shared_ptr<LockedVector<T>> tryLockAndGet () {
		if (vectorMutex.try_lock ()) {
			return std::shared_ptr<LockedVector<T>> (new LockedVector<T> (dataVector, this));
		}
		throw std::logic_error ("No lock acquired");
	}
};


#endif // LOCAKBLE_VECTOR_HPP