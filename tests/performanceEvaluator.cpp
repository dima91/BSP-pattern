
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

	BSP<int>::SuperstepPointer sj	= BSP<int>::SuperstepPointer (new Superstep<int> ());


	for (int j=0; j<15; j++) {
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

	{
		UTimer timer ("Running whole BSP pattern");
		bspPattern.runAndWait (inputVectors, outputVectors, false);
	}


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