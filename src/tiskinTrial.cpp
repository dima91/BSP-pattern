/**
 * \file tiskinTrial.cpp
 * \brief Main file containing a "static" implementation of Tiskin sorting algorithm.
 * \author Luca Di Mauro
 */


#include <bsp.hpp>

#include <iostream>
#include <random>
#include <map>

#define PRINT_V(lbl, vec, footer) {\
	std::cout << lbl << std::endl;\
	for (auto el : vec)\
		std::cout << el << " ";\
	std::cout << std::endl << footer;\
}

using CommunicationProtocolFun	= std::function<Superstep<int>::CommunicationProtocol (std::vector<int> &)>;

std::atomic_int k;

std::map<int, std::vector<int>> vectorsMap;
std::mutex mapMutex;


void setupArray (std::vector<int> &input, int seed) {
    std::mt19937 mt (seed);
	std::uniform_int_distribution<int> dist (1, 100);

	for (size_t i=0; i<input.size(); i++) {
		int nextInt;
		do {
			nextInt	= dist(mt);
		} while (std::find(input.begin(), input.end(), nextInt) != input.end());
		input[i]	= nextInt;
	}
}




void setupBsp (BSP<int> &tt, std::vector<int> &input, std::vector<std::vector<int>> &bspInputs, std::vector<std::vector<int>> &bspOutputs) {
	setupArray (input, 12);

	bspInputs	= std::vector<std::vector<int>> (3);
	bspOutputs	= std::vector<std::vector<int>> (3);

	for (int i=0; i<3; i++) {
		bspInputs[i]	= std::vector<int> (7);
		auto &vI		= bspInputs[i];
		for (int j=0; j<7; j++) {
			vI[j]	= input[(7*i)+j];
		}
	}




	/* ==============================    ============================== */
	/*								  s0								*/
	/* ==============================    ============================== */

	BSP<int>::SuperstepPointer s0	= BSP<int>::SuperstepPointer (new Superstep<int> ());

	for (int i : {0, 1, 2}) {
		s0->addActivity (
			[] (int activityIndex, std::vector<int> &actInput) {
				std::sort (actInput.begin(), actInput.end());
				vectorsMap[activityIndex]	= actInput;
			},
			[] (int activityIndex, std::vector<int> &elements) {
				Superstep<int>::CommunicationProtocol cp (3);
				for (auto i :{0,1,2}) {
					cp[i]	= std::vector<int> (4);
					cp[i][0]	= elements[0];
					cp[i][1]	= elements[2];
					cp[i][2]	= elements[4];
					cp[i][3]	= elements[6];
				}
				return cp;
			}
		);
	}

	tt.addSuperstep (std::move(s0));




	/* ==============================    ============================== */
	/*								  s1								*/
	/* ==============================    ============================== */

	BSP<int>::SuperstepPointer s1	= BSP<int>::SuperstepPointer (new Superstep<int> ());

	for (size_t i : {0, 1, 2}) {
		s1->addActivity (
			[] (int activityIndex, std::vector<int> &actInput) {
				std::sort (actInput.begin(), actInput.end());
			},
			([] (int activityIndex, std::vector<int> &els) {
				std::vector<std::vector<int>> cp (3);
				mapMutex.lock ();
				std::vector<int> &pi	= vectorsMap[activityIndex];
				mapMutex.unlock ();
				size_t i				= 0;
				
				//cp[0]	= std::vector<int> ();
				while (pi[i] <= 30 && i<pi.size()) {
					cp[0].push_back (pi[i]);
					i++;
				}

				cp[1]	= std::vector<int> ();
				while (pi[i] <= 73 && i<pi.size()) {
					cp[1].push_back (pi[i]);
					i++;
				}

				cp[2]	= std::vector<int> ();
				while (pi[i] <= 99 && i<pi.size()) {
					cp[2].push_back (pi[i]);
					i++;
				}

				return cp;
			})
		);
	}

	tt.addSuperstep (std::move(s1));




	/* ==============================    ============================== */
	/*								  s2								*/
	/* ==============================    ============================== */

	BSP<int>::SuperstepPointer s2	= BSP<int>::SuperstepPointer (new Superstep<int> ());

	for (int i : {0, 1, 2}) {
		s2->addActivity (
			[] (int activityIndex, std::vector<int> &actInput) {
				std::sort (actInput.begin(), actInput.end());
			},
			([] (int activityIndex, std::vector<int> &elements) {
				std::vector<std::vector<int>> cp (3);
				for (auto el: elements) {
					cp[activityIndex].push_back (el);
				}
				return cp;
			})
		);
	}

	tt.addSuperstep (std::move(s2));
}








int main (int argn, char **argv) {
	std::cout << "Hwllo user! Welcome to 'static' implementation of Tiskin alogirthm\n\n";
	
	BSP<int> tiskinTrial;
	int n	= 21;					// Number of element in the array
	//int p	= 3;					// Number of concurrent activities
	std::vector<int> input (n);		// Array containing input elements
	std::vector<int> output (n);	// Array containing ouput elements
	std::vector<std::vector<int>> bspInputs;
	std::vector<std::vector<int>> bspOutputs;

	setupBsp (tiskinTrial, std::ref(input), std::ref(bspInputs), std::ref(bspOutputs));

	/*PRINT_V ("First_I", bspInputs[0], "");
	PRINT_V ("Second_I", bspInputs[1], "");
	PRINT_V ("Third_I", bspInputs[2], "\n");*/

	std::cout << "Input vector:\n";
	for (auto bspI : bspInputs) {
		for (auto el : bspI)
			std::cout << el << " ";
	}

	tiskinTrial.runAndWait (std::ref(bspInputs), std::ref(bspOutputs), false);

	std::cout << "\n\n============================================================\n\n";
	/*PRINT_V ("First_O", bspOutputs[0], "");
	PRINT_V ("Second_O", bspOutputs[1], "");
	PRINT_V ("Third_O", bspOutputs[2], "\n");*/

	std::cout << "Sorted vector:\n";
	for (auto bspO : bspOutputs) {
		for (auto el : bspO)
			std::cout << el << " ";
	}
	std::cout << std::endl;

	return 0;
}