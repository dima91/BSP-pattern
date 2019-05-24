
#include <bsp.hpp>
#include <uTimer.hpp>

#include <iostream>
#include <algorithm>
#include <random>


using IntActivityFunction			= Superstep<int>::ActivityFunction;
using IntCommunicationProtocol		= Superstep<int>::CommunicationProtocol;
using IntCommunicationProtocolFun	= std::function<Superstep<int>::CommunicationProtocol (int, std::vector<int> &)>;



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
				cp[(i+1)%4] = {0, 1};

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
			cp[(i-1+4)%4] = {0, 1};

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

	bspPattern.runAndWait (inputVectors, outputVectors);



	std::cout << "Output vectors..\n";
	for (size_t i=0; i<outputVectors.size(); i++) {
		std::cout << i << " >  ";
		for (auto el : outputVectors[i])
			std::cout << el << "\t";
		std::cout << std::endl;
	}

	return 0;
}