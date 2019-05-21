
#include <superstep.hpp>
#include <lockableVector.hpp>
#include <workerThread.hpp>

#include <iostream>
#include <future>
#include <algorithm>

using namespace std;


void s0Function (std::vector<int>& v) {
	for_each (v.begin(), v.end(), [] (int i) {
		cout << "-->  " << i << endl;
	});
	
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
	Superstep<int> s0 (s0Function, parDeg, comProto);

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




	return 0;
}