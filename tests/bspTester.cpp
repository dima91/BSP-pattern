
#include <bsp.hpp>
#include <uTimer.hpp>

#include <iostream>
#include <algorithm>
#include <random>

//#define BSP_TESTER_0
//#define BSP_TESTER_1
#define PAR_SUPERSTEP


#ifdef BSP_TESTER_0


using IntActivityFunction			= Superstep<int>::ActivityFunction;
using IntCommunicationProtocol		= Superstep<int>::CommunicationProtocols;
using IntCommunicationProtocolFun	= std::function<Superstep<int>::CommunicationProtocols (int, std::vector<int> &)>;



int main (int argn, char **argv) {
	std::cout << "Hello user!\n";

	BSP<int> bspPattern;

	BSP<int>::SuperstepPointer s0	= BSP<int>::SuperstepPointer (new Superstep<int> ());
	BSP<int>::SuperstepPointer s1	= BSP<int>::SuperstepPointer (new Superstep<int> ());


	for (int i=0; i<4; i++) {
		// ==============
		// First activity
		IntActivityFunction aFun	= IntActivityFunction ([] (int actIdx, std::vector<int> &inputs) {
			for (size_t i=0; i<inputs.size(); i++) {
				inputs[i]	= pow (inputs[i], 2);
			}
			return ;
		});
		auto aComFun	= IntCommunicationProtocolFun ([i] (int actIdx, std::vector<int> els) {
				std::random_device random_device;
				std::mt19937 engine{random_device()};
				std::uniform_int_distribution<int> dist(1000, 1001);
				//std::this_thread::sleep_for (std::chrono::milliseconds (dist(engine)));

				IntCommunicationProtocol cp (4);
				cp[(i+1)%4] = {els[0], els[1]};

				return cp;
		});
		s0->addActivity (aFun, aComFun);


		// ===============
		// Second activity
		IntActivityFunction bFun	= IntActivityFunction ([] (int actIdx, std::vector<int> &inputs) {
			for (size_t i=0; i<inputs.size(); i++) {
				inputs[i]	= pow (inputs[i], 0.5);
			}
			return ;
		});
		auto bComFun	= IntCommunicationProtocolFun ([i] (int actIdx, std::vector<int> els) {
			std::random_device random_device;
			std::mt19937 engine{random_device()};
			std::uniform_int_distribution<int> dist(100, 300);
			//std::this_thread::sleep_for (std::chrono::milliseconds (dist(engine)));

			IntCommunicationProtocol cp (4);
			cp[(i-1+4)%4] = {els[0], els[1]};

			return cp;
		});
		s1->addActivity (bFun, bComFun);
	}

	s0->setAtExitFunction ([] (std::vector<LockableVector<int>> &outV) {
		return -1;
	});
	/*s1->setAtExitFunction ([] (std::vector<LockableVector<int>> &outV) {
		return 0;
	});*/

	std::cout << "Superstep id:  " << bspPattern.addSuperstep (std::move(s0)) << std::endl;
	std::cout << "Superstep id:  " << bspPattern.addSuperstep (std::move(s1)) << std::endl;



	std::vector<std::vector<int>>  inputVectors (4),  outputVectors (4);

	inputVectors[0].push_back (1);
	inputVectors[0].push_back (2);
	inputVectors[1].push_back (3);
	inputVectors[1].push_back (4);
	inputVectors[2].push_back (5);
	inputVectors[2].push_back (6);
	inputVectors[3].push_back (7);
	inputVectors[3].push_back (8);

	std::cout << "Starting input vectors..\n";
	for (size_t i=0; i<inputVectors.size(); i++) {
		std::cout << i << " >  ";
		for (auto el : inputVectors[i])
			std::cout << el << "\t";
		std::cout << std::endl;
	}

	bspPattern.runAndWait (inputVectors, outputVectors, false);



	std::cout << "Output vectors..\n";
	for (size_t i=0; i<outputVectors.size(); i++) {
		std::cout << i << " >  ";
		for (auto el : outputVectors[i])
			std::cout << el << "\t";
		std::cout << std::endl;
	}

	return 0;
}


#endif




#ifdef BSP_TESTER_1


#define TISKIN_PRINT_V(lbl, vec, footer) {\
	std::unique_lock<std::mutex> lock (outputMutex);\
	std::cout << lbl << std::flush;\
	for (auto el : vec)\
		std::cout << el << " " << std::flush;\
	std::cout << std::endl << footer;\
}

using IntVector					= std::vector<int>;
using IntCommunicationProtocol	= Superstep<int>::CommunicationProtocols;

std::mutex outputMutex;


void setupBsp (BSP<int> &sTester, IntVector &input,	std::vector<IntVector> &bspInputs, std::vector<IntVector> &bspOutputs) {
	
	std::random_device randomDevice;
	
	{
		UTimer randomVector ("Creating random vector");
		int idx	= 0;
		std::iota (input.begin(), input.end(), idx++);
	}


	{
		UTimer bspInputVectors ("Creating BSP input vectors");
		
		bspInputs	= std::vector<IntVector> (4);
		bspOutputs	= std::vector<IntVector> (4);

		uint actInputLen	= 5;
		for (uint i=0; i<4; i++) {
			bspInputs[i]	= IntVector (actInputLen);
			auto &vI		= bspInputs[i];
			for (uint j=0; j<actInputLen; j++) {

				vI[j]	= input[(actInputLen*i)+j];
			}
		}
	}


	UTimer superstepCreator ("Creating supersteps");


	// ============================================================
	// ============================================================
	// Superstep 0

	BSP<int>::SuperstepPointer s0	= BSP<int>::SuperstepPointer (new Superstep<int> ());

	for (uint i= 0; i<4; i++) {
		s0->addActivity (
			[] (uint activityIndex, IntVector &actInput) {
				TISKIN_PRINT_V ("Input vector of activity " << activityIndex << ": ", actInput, "");
			},
			[] (uint activityIndex, IntVector &elements) {
				//TISKIN_PRINT_V ("Input vector of activity " << activityIndex << ": ", actInput, "");
				IntCommunicationProtocol cp (5);
				for (auto el: elements) {
					cp[activityIndex].push_back (el);
					cp[activityIndex+1].push_back (el);
				}

				TISKIN_PRINT_V ("Output vector from " << activityIndex << " to " << activityIndex << ": ", cp[activityIndex], "");
				TISKIN_PRINT_V ("Output vector from " << activityIndex << " to " << activityIndex+1 << ": ", cp[activityIndex+1], "");
				

				return cp;
			}
		);
	}

	sTester.addSuperstep (std::move(s0));


	// ============================================================
	// ============================================================
	// Superstep 1 

	BSP<int>::SuperstepPointer s1	= BSP<int>::SuperstepPointer (new Superstep<int> ());

	for (uint i= 0; i<5; i++) {
		s1->addActivity (
			[] (uint activityIndex, IntVector &actInput) {
				TISKIN_PRINT_V ("Input vector of activity " << activityIndex << ": ", actInput, "");
			},
			([] (uint activityIndex, IntVector &elements) {
				//TISKIN_PRINT_V ("Input vector of activity " << activityIndex << ": ", actInput, "");
				std::vector<IntVector> cp (1);
				for (auto el: elements) {
					cp[0].push_back (el);
				}

				return cp;
			})
		);
	}

	sTester.addSuperstep (std::move(s1));


	// ============================================================
	// ============================================================
	// Superstep 2

	BSP<int>::SuperstepPointer s2	= BSP<int>::SuperstepPointer (new Superstep<int> ());

	s2->addActivity (
		[] (uint activityIndex, IntVector &actInput) {
			TISKIN_PRINT_V ("Input vector of activity " << activityIndex << ": ", actInput, "");
			std::cout << "Len: " << actInput.size() << std::endl;
		},
		([] (uint activityIndex, IntVector &elements) {
			std::vector<IntVector> cp (1);

			for (auto e : elements)
				cp[0].push_back (e);

			return cp;
		})
	);

	sTester.addSuperstep (std::move(s2));
}



int main (int argn, char **argv) {

	IntVector input (20);
	std::vector<IntVector> bspInputs, bspOutputs;
	BSP<int> bspPattern;

	setupBsp (std::ref(bspPattern), input, bspInputs, bspOutputs);

	bspPattern.runAndWait (bspInputs, bspOutputs, false);

	std::cout << std::endl << std::endl << bspOutputs.size () << std::endl;

	std::cout << "\nBye bye!\n";

	return 0;
}


#endif




#ifdef PAR_SUPERSTEP

using IntActivityFunction			= Superstep<int>::ActivityFunction;
using IntCommunicationProtocol		= Superstep<int>::CommunicationProtocols;
using IntCommunicationProtocolFun	= std::function<Superstep<int>::CommunicationProtocols (int, std::vector<int> &)>;



int main (int argn, char **argv) {
	std::cout << "Hello user!\n";

	BSP<int> bspPattern;

	BSP<int>::SuperstepPointer s0	= BSP<int>::SuperstepPointer (new Superstep<int> ());
	BSP<int>::SuperstepPointer s1	= BSP<int>::SuperstepPointer (new Superstep<int> ());


	for (int i=0; i<4; i++) {
		// ==============
		// First activity
		IntActivityFunction aFun	= IntActivityFunction ([] (int actIdx, std::vector<int> &inputs) {
			for (size_t i=0; i<inputs.size(); i++) {
				inputs[i]	= pow (inputs[i], 2);
			}
			return ;
		});
		auto aComFun	= IntCommunicationProtocolFun ([i] (int actIdx, std::vector<int> els) {
				std::random_device random_device;
				std::mt19937 engine{random_device()};
				std::uniform_int_distribution<int> dist(1000, 1001);
				//std::this_thread::sleep_for (std::chrono::milliseconds (dist(engine)));

				IntCommunicationProtocol cp (4);
				cp[(i+1)%4] = {els[0], els[1]};

				return cp;
		});
		s0->addActivity (aFun, aComFun);


		// ===============
		// Second activity
		IntActivityFunction bFun	= IntActivityFunction ([] (int actIdx, std::vector<int> &inputs) {
			for (size_t i=0; i<inputs.size(); i++) {
				inputs[i]	= pow (inputs[i], 0.5);
			}
			return ;
		});
		auto bComFun	= IntCommunicationProtocolFun ([i] (int actIdx, std::vector<int> els) {
			std::random_device random_device;
			std::mt19937 engine{random_device()};
			std::uniform_int_distribution<int> dist(100, 300);
			//std::this_thread::sleep_for (std::chrono::milliseconds (dist(engine)));

			IntCommunicationProtocol cp (4);
			cp[(i-1+4)%4] = {els[0], els[1]};

			return cp;
		});
		s1->addActivity (bFun, bComFun);
	}

	s0->setAtExitFunction ([] (std::vector<LockableVector<int>> &outV) {
		return -1;
	});
	/*s1->setAtExitFunction ([] (std::vector<LockableVector<int>> &outV) {
		return 0;
	});*/

	std::cout << "Superstep id:  " << bspPattern.addSuperstep (std::move(s0)) << std::endl;
	std::cout << "Superstep id:  " << bspPattern.addSuperstep (std::move(s1)) << std::endl;



	std::vector<std::vector<int>>  inputVectors (4),  outputVectors (4);

	inputVectors[0].push_back (1);
	inputVectors[0].push_back (2);
	inputVectors[1].push_back (3);
	inputVectors[1].push_back (4);
	inputVectors[2].push_back (5);
	inputVectors[2].push_back (6);
	inputVectors[3].push_back (7);
	inputVectors[3].push_back (8);

	std::cout << "Starting input vectors..\n";
	for (size_t i=0; i<inputVectors.size(); i++) {
		std::cout << i << " >  ";
		for (auto el : inputVectors[i])
			std::cout << el << "\t";
		std::cout << std::endl;
	}

	bspPattern.runAndWait (inputVectors, outputVectors, false);



	std::cout << "Output vectors..\n";
	for (size_t i=0; i<outputVectors.size(); i++) {
		std::cout << i << " >  ";
		for (auto el : outputVectors[i])
			std::cout << el << "\t";
		std::cout << std::endl;
	}

	return 0;
}


#endif