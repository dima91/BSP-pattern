
#include <superstep.hpp>
#include <lockableVector.hpp>
#include <workerThread.hpp>

#include <iostream>
#include <future>
#include <algorithm>
#include <random>

using namespace std;
using IntActivityFunction			= Superstep<int>::ActivityFunction;
using IntCommunicationProtocol		= Superstep<int>::CommunicationProtocol;
using IntCommunicationProtocolFun	= std::function<Superstep<int>::CommunicationProtocol (std::vector<int> &)>;



void s0Function (std::vector<int>& v) {
	for (size_t i=0; i<v.size(); i++) {
		v[i] *= v[i];
	}
	return ;
}



int main (int argn, char **argv) {
	cout << "Hello user!!\n";

	int parDeg	= 4;

	if (argn == 2)
		parDeg	= std::atoi (argv[1]);


	auto runnerF = std::function<void (std::vector<WorkerThread> &, std::vector<std::vector<int>> &, std::vector<LockableVector<int>> &, Superstep<int> &)> (
	[] (std::vector<WorkerThread> &workers, std::vector<std::vector<int>> &inV, std::vector<LockableVector<int>> &outV, Superstep<int> &sstep) {
		sstep.runStep (workers, inV, outV);

		/*for (size_t h=0; h<inputVectors.size(); h++) {
			inputVectors[h].clear ();
			std::cout << "s " << inputVectors[h].size() << endl;
			std::cout << "c " << inputVectors[h].capacity() << endl;
		}*/

		std::cout << "Edited input vectors..\n";
		for (size_t i=0; i<inV.size(); i++) {
			cout << i << " >  ";
			for (auto el : inV[i])
				cout << el << "\t";
			std::cout << endl;
		}

		std::cout << "Output vectors..\n";
		for (size_t i=0; i<outV.size(); i++) {
			cout << i << " >  ";
			for (auto el : outV[i].getVector())
				cout << el << "\t";
			std::cout << endl;
		}
	});




	std::vector<WorkerThread> workers (parDeg);
	std::vector<std::vector<int>> inputVectors (parDeg);
	std::vector<LockableVector<int>> outputVectors (parDeg);

	std::vector<std::vector<int>> comProto (parDeg);
	Superstep<int> s0;
	Superstep<int> s1;

	for (int i=0; i<parDeg; i++) {
		// ==============
		// First activity
		IntActivityFunction aFun	= IntActivityFunction ([] (std::vector<int> &inputs) {
			for (size_t i=0; i<inputs.size(); i++) {
				inputs[i]	= pow (inputs[i], 2);
			}
			return ;
		});
		auto aComFun	= IntCommunicationProtocolFun ([parDeg, i] (std::vector<int> els) {
				std::random_device random_device;
				std::mt19937 engine{random_device()};
				std::uniform_int_distribution<int> dist(200, 1000);
				std::this_thread::sleep_for (std::chrono::milliseconds (dist(engine)));

				IntCommunicationProtocol cp (parDeg);
				cp[(i+1)%parDeg] = {0, 1};

				return cp;
		});
		s0.addActivity (aFun, aComFun);


		// ===============
		// Second activity
		IntActivityFunction bFun	= IntActivityFunction ([] (std::vector<int> &inputs) {
			for (size_t i=0; i<inputs.size(); i++) {
				inputs[i]	= pow (inputs[i], 0.5);
			}
			return ;
		});
		auto bComFun	= IntCommunicationProtocolFun ([parDeg, i] (std::vector<int> els) {
			std::random_device random_device;
			std::mt19937 engine{random_device()};
			std::uniform_int_distribution<int> dist(200, 1000);
			std::this_thread::sleep_for (std::chrono::milliseconds (dist(engine)));

			IntCommunicationProtocol cp (parDeg);
			cp[(i-1+parDeg)%parDeg] = {0, 1};

			return cp;
		});
		s1.addActivity (bFun, bComFun);
	}

	if (parDeg>0) {
		inputVectors[0].push_back (2);
		inputVectors[0].push_back (4);
	}
	if (parDeg>1) {
		inputVectors[1].push_back (6);
		inputVectors[1].push_back (8);
	}
	if (parDeg>2) {
		inputVectors[2].push_back (1);
		inputVectors[2].push_back (3);
	}
	if (parDeg>3) {
		inputVectors[3].push_back (5);
		inputVectors[3].push_back (7);
	}
	if (parDeg>4)
		throw std::runtime_error ("Wrong number of parDeg");


	std::cout << "Starting input vectors..\n";
		for (size_t i=0; i<inputVectors.size(); i++) {
			cout << i << " >  ";
			for (auto el : inputVectors[i])
				cout << el << "\t";
			std::cout << endl;
		}


	runnerF (workers, inputVectors, outputVectors, s0);

	std::cout << std::endl << std::endl << "Second superstep..\n\n\n";

	// Swapping vectors
	for (size_t i=0; i< inputVectors.size(); i++) {
		outputVectors[i].swap (inputVectors[i]);
	}

	for (size_t i=0; i< inputVectors.size(); i++) {
		outputVectors[i].getVector().clear ();
	}

	runnerF (workers, inputVectors, outputVectors, s1);

	std::for_each (workers.begin(), workers.end(), [] (WorkerThread& wt) {wt.stopWorker();});



	return 0;
}