
#include <workerThread.hpp>

#include <iostream>
#include <future>

using namespace std;

void summer (int x, int y) {
	//std::cout << x << " + " << y << " = " << x+y << endl;
	std::cout << "Input is   (" << x << ", " << y << ")" << endl;
	std::cout << "Result is  " << x+y << std::endl;
}



int main (int argn, char **argv) {
	WorkerThread worker;
	WorkerThread::Task aT	= WorkerThread::Task (std::bind (summer, 10, 20));

	this_thread::sleep_for (chrono::milliseconds (2000));

	std::cout << "Assigning activity\n";
	worker.assignActivity (std::move(aT));

	this_thread::sleep_for (chrono::milliseconds (2000));

	cout << "Stopping worker..\n";
	worker.stopWorker ();

	cout << "Exiting..\n";

	return 0;
}