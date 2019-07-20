/**
 * \file bspTester.cpp
 * \brief Main file containing two tests of BSP class.
 * \author Luca Di Mauro
 */

#include <bsp.hpp>
#include <uTimer.hpp>

#include <iostream>
#include <algorithm>
#include <random>

#define BSP_PRINT_V(lbl, vec, footer) {\
	{\
		std::unique_lock<std::mutex> lock (outputMutex);\
		std::cout << lbl << std::flush;\
		for (auto el : vec)\
			std::cout << el << " " << std::flush;\
		std::cout << std::endl << footer;\
	}\
}


using IntActivityFunction			= Superstep<int>::ActivityFunction;
using IntCommunicationProtocol		= Superstep<int>::CommunicationProtocols;
using IntCommunicationProtocolFun	= std::function<Superstep<int>::CommunicationProtocols (int, std::vector<int> &)>;


// Function to test possibility to customize 'atExit' superstep function
void testAtExitFunction ();

// Function to test different number of activities in different supersteps
void testDifferentNumber ();


std::mutex outputMutex;



int main (int argn, char **argv) {
	
	testAtExitFunction ();

	testDifferentNumber ();

	return 0;
}








/* This function creates and executes a BSP object which contains three supersteps and register the 'atExit' function for
 * the second superstep which implies the exit from computation without execute the third superstep
 */
void testAtExitFunction () {

	BSP<int> bspPattern;

	BSP<int>::SuperstepPointer s0	= BSP<int>::SuperstepPointer (new Superstep<int> ());
	BSP<int>::SuperstepPointer s1	= BSP<int>::SuperstepPointer (new Superstep<int> ());
	BSP<int>::SuperstepPointer s2	= BSP<int>::SuperstepPointer (new Superstep<int> ());


	for (int i=0; i<2; i++) {
		// ==============
		// First activity
		auto aFun		= IntActivityFunction ([] (int actIdx, std::vector<int> &inputs) {
			for (size_t i=0; i<inputs.size(); i++) {
				inputs[i]	= pow (inputs[i], 2);
			}
			return ;
		});
		auto aComFun	= IntCommunicationProtocolFun ([] (int actIdx, std::vector<int> els) {
				IntCommunicationProtocol cp (2);
				cp[(actIdx+1)%2] = {els[0], els[1]};
				return cp;
		});
		s0->addActivity (aFun, aComFun);


		// ===============
		// Second activity
		auto bFun		= IntActivityFunction ([] (int actIdx, std::vector<int> &inputs) {
			for (size_t i=0; i<inputs.size(); i++) {
				inputs[i]	= pow (inputs[i], 0.5);
			}
			return ;
		});
		auto bComFun	= IntCommunicationProtocolFun ([] (int actIdx, std::vector<int> els) {
			IntCommunicationProtocol cp (2);
			cp[(actIdx-1+2)%2] = {els[0], els[1]};
			return cp;
		});
		s1->addActivity (bFun, bComFun);
	}


	/* Creating a third activity which wll never executed */
	auto compFun	= IntActivityFunction ([] (int actIdx, std::vector<int> &inputs) {
		std::cout << "This computation function will not be executed\n";
		return ;
	});
	auto commFun	= IntCommunicationProtocolFun ([] (int actIdx, std::vector<int> els) {
		IntCommunicationProtocol cp (2);
		std::cout << "This communication function will not be executed\n";
		return cp;
	});
	s2->addActivity (compFun, commFun);

	s1->setAtExitFunction ([] (std::vector<LockableVector<int>> &outV) {
		return -1;
	});

	std::cout << "Inserted superstep with id:  " << bspPattern.addSuperstep (std::move(s0)) << std::endl;
	std::cout << "Inserted superstep with id:  " << bspPattern.addSuperstep (std::move(s1)) << std::endl;
	std::cout << "Inserted superstep with id:  " << bspPattern.addSuperstep (std::move(s2)) << std::endl;



	std::vector<std::vector<int>>  inputVectors (2),  outputVectors (2);

	inputVectors[0].push_back (1);
	inputVectors[0].push_back (2);
	inputVectors[1].push_back (3);
	inputVectors[1].push_back (4);

	bspPattern.runAndWait (inputVectors, outputVectors, false);
}








/* Thsi function creates and executes a BSP object which contains thre supersteps: the first one containing three activities,
 * the second one containing four activities and the third one containing one activity. It tests the possibility to create
 * supersteps with different numbers of activities inside the same BSP object
 */
void testDifferentNumber () {
	std::vector<int> input (20);
	std::vector<std::vector<int>> bspInputs, bspOutputs;
	BSP<int> bspPattern;

	std::random_device randomDevice;

	int idx	= 0;
	std::iota (input.begin(), input.end(), idx++);
	
	bspInputs	= std::vector<std::vector<int>> (2);
	bspOutputs	= std::vector<std::vector<int>> (1);

	uint actInputLen	= 5;
	for (uint i=0; i<2; i++) {
		bspInputs[i]	= std::vector<int> (actInputLen);
		auto &vI		= bspInputs[i];
		for (uint j=0; j<actInputLen; j++) {

			vI[j]	= input[(actInputLen*i)+j];
		}
	}


	// ============================================================
	// ============================================================
	// Superstep 0

	BSP<int>::SuperstepPointer s0	= BSP<int>::SuperstepPointer (new Superstep<int> ());

	for (uint i= 0; i<2; i++) {
		s0->addActivity (
			[] (uint activityIndex, std::vector<int> &actInput) {
				BSP_PRINT_V ("Input vector of activity " << activityIndex << ": ", actInput, "");
			},
			[] (uint activityIndex, std::vector<int> &elements) {
				IntCommunicationProtocol cp (3);
				for (auto el: elements) {
					cp[activityIndex].push_back (el);
					cp[activityIndex+1].push_back (el);
				}
				return cp;
			}
		);
	}

	bspPattern.addSuperstep (std::move(s0));


	// ============================================================
	// ============================================================
	// Superstep 1 

	BSP<int>::SuperstepPointer s1	= BSP<int>::SuperstepPointer (new Superstep<int> ());

	for (uint i= 0; i<3; i++) {
		s1->addActivity (
			[] (uint activityIndex, std::vector<int> &actInput) {
				BSP_PRINT_V ("Input vector of activity " << activityIndex << ": ", actInput, "");
			},
			([] (uint activityIndex, std::vector<int> &elements) {
				//TISKIN_PRINT_V ("Input vector of activity " << activityIndex << ": ", actInput, "");
				std::vector<std::vector<int>> cp (1);
				for (auto el: elements) {
					cp[0].push_back (el);
				}

				return cp;
			})
		);
	}

	bspPattern.addSuperstep (std::move(s1));


	// ============================================================
	// ============================================================
	// Superstep 2
	BSP<int>::SuperstepPointer s2	= BSP<int>::SuperstepPointer (new Superstep<int> ());

	s2->addActivity (
		[] (uint activityIndex, std::vector<int> &actInput) {
			BSP_PRINT_V ("Input vector of activity " << activityIndex << ": ", actInput, "");
		},
		([] (uint activityIndex, std::vector<int> &elements) {
			std::vector<std::vector<int>> cp (1);

			for (auto e : elements)
				cp[0].push_back (e);

			return cp;
		})
	);

	bspPattern.addSuperstep (std::move(s2));

	bspPattern.runAndWait (bspInputs, bspOutputs, false);
}