
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
		std::vector<int> &lockedV	= locV.lockAndGet ();
		cout << "[0] Locked\n";
		lockedV.push_back (10);
		lockedV.push_back (20);
		this_thread::sleep_for (chrono::milliseconds (2000));
		cout << "[0] Unlocking\n";
		locV.releaseVector ();
		cout << "[0] Unlocked\n";
	});
	
	
	auto handle1 = async (launch::async, [&locV]() {
		this_thread::sleep_for (chrono::milliseconds (10));
		cout << "[1] Locking\n";
		std::vector<int> &lockedV	= locV.lockAndGet ();
		cout << "[1] Locked\n";
		int zero	= std::move (lockedV[0]);
		int uno		= std::move (lockedV[1]);
		lockedV.erase (lockedV.begin());
		cout << "zero:  " << zero << " --" << lockedV[0] << "--\n";
		cout << "uno:   " << uno  << " --" << lockedV[1] << "--\n";
		cout << "size:  " << lockedV.size () << endl;
		lockedV.erase (lockedV.begin());
		cout << "size:  " << lockedV.size () << endl;
		std::for_each (lockedV.begin(), lockedV.end(), [] (int i) {
			cout << "--> " << i << endl;
		});
		cout << "[1] Unlocking\n";
		locV.releaseVector ();
		cout << "[1] Unlocked\n";
	});

	handle0.wait ();
	handle1.wait ();

	return 0;
}