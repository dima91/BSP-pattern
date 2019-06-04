
//#define MAP_TESTER				// First implementation
#define MAP_OF_SEQ_TESTER		// Second implementaton


#ifdef MAP_TESTER


#include <bsp.hpp>
#include <uTimer.hpp>

#include <iostream>
#include <algorithm>
#include <random>

using namespace std::chrono;

using IntActivityFunction			= Superstep<int>::ActivityFunction;
using IntCommunicationProtocol		= Superstep<int>::CommunicationProtocols;
using IntCommunicationProtocolFun	= std::function<Superstep<int>::CommunicationProtocols (int, std::vector<int> &)>;
using ms							= milliseconds;

#define COMP_PHASE_DELAY 80 // ms
#define COMM_PHASE_DELAY 20 // ms




int main (int argn, char **argv) {
	std::cout << "Hello user!\n";

	BSP<int> bspPattern;


	for (int j=0; j<4; j++) {
		BSP<int>::SuperstepPointer sj	= BSP<int>::SuperstepPointer (new Superstep<int> ());
		for (int i=0; i<4; i++) {
			// ==============
			// First activity
			IntActivityFunction aFun	= IntActivityFunction ([] (int actIdx, std::vector<int> &inputs) {
				auto start	= high_resolution_clock::now();
				do {} while (duration_cast<ms>(high_resolution_clock::now() - start).count () < COMP_PHASE_DELAY);
				return ;
			});
			auto aComFun	= IntCommunicationProtocolFun ([i] (int actIdx, std::vector<int> els) {
				IntCommunicationProtocol cp;
				auto start	= high_resolution_clock::now();
				do {} while (duration_cast<ms>(high_resolution_clock::now() - start).count () < COMM_PHASE_DELAY);
				return cp;
			});
			sj->addActivity (aFun, aComFun);
		}
		std::cout << "Superstep id:  " << bspPattern.addSuperstep (std::move(sj)) << std::endl;
	}



	std::vector<std::vector<int>>  inputVectors (4),  outputVectors (4);

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


#endif	// MAP_TESTER


#ifdef MAP_OF_SEQ_TESTER
#define MAP_OF_SEQ_MODEL

#include <bsp.hpp>
#include <uTimer.hpp>

#include <iostream>
#include <algorithm>
#include <random>

using namespace std::chrono;

using IntActivityFunction			= Superstep<int>::ActivityFunction;
using IntCommunicationProtocol		= Superstep<int>::CommunicationProtocols;
using IntCommunicationProtocolFun	= std::function<Superstep<int>::CommunicationProtocols (int, std::vector<int> &)>;
using ms							= milliseconds;

#define COMP_PHASE_DELAY 80 // ms
#define COMM_PHASE_DELAY 20 // ms




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


#endif	// MAP_OF_SEQ_TESTER