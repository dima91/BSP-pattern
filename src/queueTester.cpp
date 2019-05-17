
#include <concurrentQueue.hpp>

#include <iostream>
#include <future>

using namespace std;



int main (int argn, char **argv) {
	ConcurrentQueue<string> cq;
	

	auto handle2 = async (launch::async, [&cq]() {
		this_thread::sleep_for (chrono::milliseconds (2000));
		cq.push ("Hello");
		cout << "Pushed..\n";
	});
	
	
	auto handle1 = async (launch::async, [&cq]() {
		this_thread::sleep_for (chrono::milliseconds (1000));
		string popped;
		cq.pop (popped);
		cout << "popped: " << popped << endl;
	});

	return 0;
}