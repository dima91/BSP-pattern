//#define MAP_TESTER			// First implementation
//#define MAP_OF_SEQ_TESTER		// Second implementaton
#define ACTIVITIES_NUMBER 5


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




int main (int argn, char **argv) {
	if (argn != 5) {
		std::cerr << "Usage:\t ./perfEval <input len> <number of procressors> <computation delay> <communication delay>\n";
		return EXIT_FAILURE;
	}

	std::cout << "Hello user! Running MAP_TESTER\n";


	BSP<int> bspPattern;
	int inputLen		= std::atoi (argv[1]);
	int p				= std::atoi (argv[2]);
	int compPhaseDelay	= std::atoi (argv[3]);
	int commPhaseDelay	= std::atoi (argv[4]);


	for (int j=0; j<ACTIVITIES_NUMBER; j++) {
		BSP<int>::SuperstepPointer sj	= BSP<int>::SuperstepPointer (new Superstep<int> ());
		for (int i=0; i<p; i++) {
			IntActivityFunction aFun	= IntActivityFunction ([compPhaseDelay, inputLen, p] (int actIdx, std::vector<int> &inputs) {
				for (auto i=0; i<inputLen/p; i++) {
					auto start	= high_resolution_clock::now();
					do {} while (duration_cast<ms>(high_resolution_clock::now() - start).count () < compPhaseDelay);
				}
				return ;
			});
			auto aComFun	= IntCommunicationProtocolFun ([commPhaseDelay, inputLen, p] (int actIdx, std::vector<int> els) {
				IntCommunicationProtocol cp (ACTIVITIES_NUMBER);
				for (auto i=0; i<inputLen/p; i++) {
					auto start	= high_resolution_clock::now();
					do {} while (duration_cast<ms>(high_resolution_clock::now() - start).count () < commPhaseDelay);
				}
				return cp;
			});
			sj->addActivity (aFun, aComFun);
		}
		std::cout << "Superstep id:  " << bspPattern.addSuperstep (std::move(sj)) << std::endl;
	}



	std::vector<std::vector<int>>  inputVectors (p),  outputVectors (p);

	for (size_t i=0; i<inputVectors.size(); i++) {
		inputVectors[i]	= std::vector<int> (inputLen/p);
	}

	{
		UTimer timer ("Running whole BSP pattern");
		bspPattern.runAndWait (inputVectors, outputVectors, false);
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
#include <cstdint>

using namespace std::chrono;

using IntActivityFunction			= Superstep<int>::ActivityFunction;
using IntCommunicationProtocol		= Superstep<int>::CommunicationProtocols;
using IntCommunicationProtocolFun	= std::function<Superstep<int>::CommunicationProtocols (int, int, int, int, std::vector<int> &)>;
using ms							= milliseconds;




int main (int argn, char **argv) {
	if (argn != 6) {
		std::cerr << "Usage: ./PerfEval_MS <input len> <processors> <sub_processors> <computation delay> <communication delay>\n";
		return EXIT_FAILURE;
	}


	std::cout << "Hello user! Running MAP_OF_SEQ_zTESTER\n";

	BSP<int> bspPattern;
	int inputLen		= std::atoi (argv[1]);
	int p				= std::atoi (argv[2]);
	int q				= std::atoi (argv[3]);
	int compPhaseDelay	= std::atoi (argv[4]);
	int commPhaseDelay	= std::atoi (argv[5]);


	for (int j=0; j<ACTIVITIES_NUMBER; j++) {
		BSP<int>::SuperstepPointer sj	= BSP<int>::SuperstepPointer (new Superstep<int> ());
		for (int i=0; i<p; i++) {
			auto aFun	= IntActivityFunction ([compPhaseDelay, inputLen, p, q] (int actIdx, int subWIdx, int start, int end,
																					std::vector<int> &inputs) {
				for (auto i=0; i<inputLen/(p*q); i++) {
					auto start	= high_resolution_clock::now();
					do {} while (duration_cast<ms>(high_resolution_clock::now() - start).count () < compPhaseDelay);
				}
				return ;
			});
			auto aComFun	= IntCommunicationProtocolFun ([p, q, commPhaseDelay, inputLen] (int actIdx, int subWIdx, int start,
																								int end, std::vector<int> els) {
				IntCommunicationProtocol cp (ACTIVITIES_NUMBER);
				for (auto i=0; i<inputLen/(p*q); i++) {
					auto start	= high_resolution_clock::now();
					do {} while (duration_cast<ms>(high_resolution_clock::now() - start).count () < commPhaseDelay);
				}
				return cp;
			});
			sj->addActivity (aFun, aComFun);
		}
		std::cout << "Superstep id:  " << bspPattern.addSuperstep (std::move(sj)) << std::endl;
	}


	std::vector<std::vector<int>>  inputVectors (p),  outputVectors (p);


	for (size_t i=0; i<inputVectors.size(); i++) {
		inputVectors[i]	= std::vector<int> (inputLen / p);
	}
	
	{
		UTimer timer ("Running whole BSP pattern");
		bspPattern.runAndWait (q, inputVectors, outputVectors, false);
	}

	return 0;
}


#endif	// MAP_OF_SEQ_TESTER