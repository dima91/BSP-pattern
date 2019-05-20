
#include <lockableVector.hpp>

#include <iostream>
#include <future>
#include <algorithm>

using namespace std;


int main (int argn, char **argv) {
	cout << "Hello user!!\n";

	LockableVector<int> locV;

	auto handle0 = async (launch::async, [&locV]() {
		cout << "[0] Locking\n";
		auto locked	= locV.lockAndGet ();
		cout << "[0] Locked\n";
		locked->data.push_back (10);
		locked->data.push_back (20);
		this_thread::sleep_for (chrono::milliseconds (2000));
		cout << "[0] Unlocking\n";
		locked.reset();
		cout << "[0] Unlocked\n";
		cout << "[0] Exiting\n";
	});
	
	
	auto handle1 = async (launch::async, [&locV]() {
		this_thread::sleep_for (chrono::milliseconds (10));
		cout << "[1] Locking\n";
		auto locked	= locV.lockAndGet ();
		cout << "[1] Locked\n";
		int zero	= std::move (locked->data[0]);
		int uno		= std::move (locked->data[1]);
		locked->data.erase (locked->data.begin());
		cout << "zero:  " << zero << " --" << locked->data[0] << "--\n";
		cout << "uno:   " << uno  << " --" << locked->data[1] << "--\n";
		cout << "size:  " << locked->data.size () << endl;
		locked->data.erase (locked->data.begin());
		cout << "size:  " << locked->data.size () << endl;
		std::for_each (locked->data.begin(), locked->data.end(), [] (int i) {
			cout << "--> " << i << endl;
		});
		cout << "[1] Unlocking\n";
		cout << "[1] Unlocked\n";
	});

	handle0.wait ();
	handle1.wait ();

	return 0;
}