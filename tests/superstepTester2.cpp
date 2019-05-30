#include <superstep.hpp>
#include <bsp.hpp>
#include <random>
#include <mutex>


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