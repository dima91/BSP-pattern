/**
 * \file barrierTester.cpp
 * \brief Source file for a Barrier class' test
 * \author Luca Di Mauro
 */


#include <barrier.hpp>

#include <iostream>
#include <thread>

using namespace std;


int main (int argn, char **argv) {
	cout << "Hello user!\n";

	{
		// TEST 1
		Barrier b (3);

		thread t0	= thread ([&b] () {
			b.decreaseBarrier ();
			cout << "Unlocked t0!\n";
		});

		thread t1	= thread ([&b] () {
			b.decreaseBarrier ();
			cout << "Unlocked t1!\n";
		});

		cout << "Sleeping...\n";
		std::this_thread::sleep_for (chrono::milliseconds (3000));
		cout << "Decreasing..\n";
		b.decreaseBarrier ();
		cout << "Not locked!\n";

		t0.join ();
		t1.join ();

		cout << endl << "=====  TEST 1 PASSED  =====\n\n\n";
	}


	{
		// TETST 2
		Barrier b (3);

		try {
			cout << "Sleeping...\n";
			std::this_thread::sleep_for (chrono::milliseconds (3000));
			cout << "Resetting..\n";
			b.reset (3);
		} catch (std::runtime_error &e) {
			cout << "Catched an error:  " << e.what () << endl;
		}

		cout << endl << "=====  TEST 2 PASSED  =====\n\n\n";
	}


	{
		// TEST 3
		Barrier b (3);

		thread t00	= thread ([&b] () {
			b.decreaseBarrier ();
			cout << "Unlocked t00!\n";
		});

		thread t10	= thread ([&b] () {
			b.decreaseBarrier ();
			cout << "Unlocked t10!\n";
		});

		try {
			cout << "Sleeping...\n";
			std::this_thread::sleep_for (chrono::milliseconds (3000));
			cout << "Resetting..\n";
			b.reset (3);
		} catch (std::runtime_error &e) {
			cout << "Catched an error:  " << e.what () << endl;
		}
		
		cout << "Decreasing...\n";
		b.decreaseBarrier ();
		cout << "Not locked!\n";
		cout << "Resetting..\n";
		b.reset (3);

		t00.join ();
		t10.join ();


		thread t01	= thread ([&b] () {
			std::this_thread::sleep_for (chrono::milliseconds (1000));
			b.decreaseBarrier ();
			cout << "Unlocked t01!\n";
		});

		thread t11	= thread ([&b] () {
			std::this_thread::sleep_for (chrono::milliseconds (500));
			b.decreaseBarrier ();
			cout << "Unlocked t11!\n";
		});

		thread t31	= thread ([&b] () {
			std::this_thread::sleep_for (chrono::milliseconds (750));
			b.decreaseBarrier ();
			cout << "Unlocked t31!\n";
		});

		t01.join ();
		t11.join ();
		t31.join ();

		cout << endl << "=====  TEST 3 PASSED  =====\n\n\n";
	}

	return 0;
}