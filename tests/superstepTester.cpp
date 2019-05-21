
#include <superstep.hpp>
#include <lockableVector.hpp>
#include <workerThread.hpp>

#include <iostream>
#include <future>
#include <algorithm>

using namespace std;


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

	std::vector<WorkerThread> workers (parDeg);
	std::vector<LockableVector<int>> lockableVectors (parDeg);

	std::vector<std::vector<int>> comProto (parDeg);
	Superstep<int> s0;

	for (int i=0; i<parDeg; i++) {
		Superstep<int>::ActivityFunction a	= std::function<void (std::vector<int>&)> ([] (std::vector<int> &inputs) {
			for (size_t i=0; i<inputs.size(); i++) {
				inputs[i] *= inputs[i];
			}
			return ;
		});
		auto comFun	= std::function<Superstep<int>::CommunicationProtocol (std::vector<int> &)> ([comProto] (std::vector<int> els) {
			return comProto;
		});
		//s0.addActivity (a, comFun);
		s0.addActivity (a, comProto);
	}

	if (parDeg>0) {
		lockableVectors[0].getVector().push_back (2);
		lockableVectors[0].getVector().push_back (4);
	}
	if (parDeg>1) {
		lockableVectors[1].getVector().push_back (6);
		lockableVectors[1].getVector().push_back (8);
	}
	if (parDeg>2) {
		lockableVectors[2].getVector().push_back (1);
		lockableVectors[2].getVector().push_back (3);
	}
	if (parDeg>3) {
		lockableVectors[3].getVector().push_back (5);
		lockableVectors[3].getVector().push_back (7);
	}
	if (parDeg>4)
		throw std::runtime_error ("Wrong number of parDeg");


	s0.runStep (workers, lockableVectors);
	std::for_each (workers.begin(), workers.end(), [] (WorkerThread& wt) {wt.stopWorker();});

	for (size_t i=0; i<lockableVectors.size(); i++) {
		for (auto el : lockableVectors[i].getVector())
			cout << el << ", ";
		std::cout << endl;
	}




	return 0;
}