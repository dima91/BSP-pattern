/**
 * \file tsikin.cpp
 * \brief Main file containing the implementation of Tsikin sorting algorithm.
 * \author Luca Di Mauro
 */


#include <bsp.hpp>

#include <iostream>
#include <random>
#include <map>
#include <mutex>

#define DEBUG

#ifdef DEBUG
	#define TSIKIN_PRINT_V(lbl, vec, footer) {\
		std::unique_lock<std::mutex> lock (outputMutex);\
		std::cout << lbl << std::endl;\
		for (auto el : vec)\
			std::cout << el << " ";\
		std::cout << std::endl << footer;\
	}
#else
	#define TSIKIN_PRINT_V(lbl, vec, footer) {}
#endif


using IntVector					= std::vector<int>;
using IntCommunicationProtocol	= Superstep<int>::CommunicationProtocol;
using CommunicationProtocolFun	= std::function<IntCommunicationProtocol (std::vector<int> &)>;


std::mutex outputMutex;
std::map<int, IntVector> s0VectorsMap;
std::mutex mapMutex;




void createRandomVector (IntVector &input, int seed) {
    std::mt19937 mt (seed);
	std::uniform_int_distribution<int> dist (1, input.size()*10);
	int nextInt	= 0;

	for (size_t i=0; i<input.size(); i++) {
		do {
			nextInt	= dist(mt);
		} while (std::find(input.begin(), input.end(), nextInt) != input.end());
		input[i]	= nextInt;
	}
}


void putPPlusOneElements (IntVector &target, IntVector &source, int p) {
	//TSIKIN_PRINT_V ("taking " << p << "elements from ", source, "");

	auto it		= source.begin();
	int size1	= source.size()-1;		// Number of elements to be taken into account (belonging to vector)
	int p1		= p-1;					// Number of elements to be selected
	int h		= size1/p1;				// Size of subset
    int k		= size1%p1;				// Number of subset with size h+1
	int i		= 0;					// Index of target array
	int tmpN1	= 0;

	//std::cout << h << " " << k << std::endl << std::endl;
	
	while (k--) {
		target[i++]	= *(it++);
		tmpN1		= h;
		while (tmpN1--)
			it++;
    }
    while (it != source.end()-1) {
		target[i++]	= *(it++);
		tmpN1	= h-1;
		while (tmpN1--)
			it++;
	}

	target[i]	= *(source.end()-1);

	//TSIKIN_PRINT_V ("Taken..", target, "");
}


void findOutSeparators (IntVector &target, IntVector &source, int n) {
	auto it		= source.begin();
	int size1	= source.size()-1;		// Number of elements to be taken into account (belonging to vector)
	int n1		= n-1;					// Number of elements to be selected
	int h		= size1/n1;				// Size of subset
    int k		= size1%n1;				// Number of subset with size h+1
	int i		= 0;					// Index of target array
	int tmpN1	= 0;

	//std::cout << h << " " << k << std::endl << std::endl;
	
	while (k--) {
		target[i++]	= *(it++);
		tmpN1		= h;
		while (tmpN1--)
			it++;
    }
    while (it != source.end()-1) {
		target[i++]	= *(it++);
		tmpN1	= h-1;
		while (tmpN1--)
			it++;
	}

	target[i]	= *(source.end()-1);

	//TSIKIN_PRINT_V ("Taken separators", target, "");
}


bool isSorted (IntVector &v) {
	return false;
}


void setupBsp (BSP<int> &tAlg, int n, int p, int seed, IntVector &input,
				std::vector<IntVector> &bspInputs, std::vector<IntVector> &bspOutputs) {
	std::random_device randomDevice;
	input	= IntVector	(n);
	createRandomVector (input, (seed==-1) ? randomDevice() : seed);
	TSIKIN_PRINT_V ("Input vector", input, "\n");


	bspInputs	= std::vector<IntVector> (p);
	bspOutputs	= std::vector<IntVector> (p);

	int actInputLen	= n/p;
	for (int i=0; i<p; i++) {
		bspInputs[i]	= std::vector<int> (actInputLen);
		auto &vI		= bspInputs[i];
		for (int j=0; j<actInputLen; j++) {
			vI[j]	= input[(actInputLen*i)+j];
		}

		//TSIKIN_PRINT_V ("Proc " << i << " input", bspInputs[i], "");
	}


	// ============================================================
	// ============================================================
	// Superstep 0

	BSP<int>::SuperstepPointer s0	= BSP<int>::SuperstepPointer (new Superstep<int> ());

	for (int i= 0; i<p; i++) {
		s0->addActivity (
			[] (int activityIndex, std::vector<int> &actInput) {
				std::sort (actInput.begin(), actInput.end());
				s0VectorsMap[activityIndex]	= actInput;
			},
			[p] (int activityIndex, std::vector<int> &elements) {
				Superstep<int>::CommunicationProtocol cp (p);
				cp[0]	= std::vector<int> (p+1);
				findOutSeparators (std::ref(cp[0]), std::ref(elements), p+1);
				for (int i=1; i<p; i++) {
					cp[i]	= std::vector<int> (p+1);
					cp[i]	= cp[0];
				}
				return cp;
			}
		);
	}

	tAlg.addSuperstep (std::move(s0));


	// ============================================================
	// ============================================================
	// Superstep 1 

	BSP<int>::SuperstepPointer s1	= BSP<int>::SuperstepPointer (new Superstep<int> ());

	for (int i= 0; i<p; i++) {
		s1->addActivity (
			[] (int activityIndex, std::vector<int> &actInput) {
				//TSIKIN_PRINT_V ("Input " << activityIndex, actInput, "");
				std::sort (actInput.begin(), actInput.end());
				//TSIKIN_PRINT_V ("Output " << activityIndex, actInput, "");
			},
			([p] (int activityIndex, std::vector<int> &commElements) {
				std::vector<IntVector> cp (p);
				IntVector separators (p+1);
				findOutSeparators (std::ref(separators), std::ref(commElements), p+1);
				
				mapMutex.lock ();
				std::vector<int> &pi	= s0VectorsMap[activityIndex];
				mapMutex.unlock ();
				int i					= 0;	// Index for 'pi' vector
				int j					= 1;	// Index for 'separators' vector
				int k					= 0;	// Index for 'cp' vector

				while (i < pi.size()) {
					cp[k]	= IntVector ();
					while (i < pi.size() && pi[i] <= separators[j]) {
						/*if (activityIndex == 0)
							std::cout << "Pushing to " << k << " element " << pi[i] << std::endl;*/
						cp[k].push_back (pi[i]);
						i++;
					}
					j++;
					k++;
				}

				return cp;
			})
		);
	}

	tAlg.addSuperstep (std::move(s1));


	// ============================================================
	// ============================================================
	// Superstep 2

	BSP<int>::SuperstepPointer s2	= BSP<int>::SuperstepPointer (new Superstep<int> ());

	for (int i= 0; i<p; i++) {
		s2->addActivity (
			[] (int activityIndex, std::vector<int> &actInput) {
				std::sort (actInput.begin(), actInput.end());
			},
			([p] (int activityIndex, std::vector<int> &elements) {
				std::vector<std::vector<int>> cp (p);
				for (auto el: elements) {
					cp[activityIndex].push_back (el);
				}
				return cp;
			})
		);
	}

	tAlg.addSuperstep (std::move(s2));
}




int main (int argn, char **argv) {
	std::string usageStr	= 	"Usage: ./bin/TsikinAlgorithm <n> <p> [seed]"
								"\n\t\tn\tNumber of items contained in the vector to be ordered"
								"\n\t\tp\tNumber of processor (parallel activities)"
								"\n\t\tseed\tSeed to be given as arguments to generate random numbers";

	if (argn < 3 || argn > 4) {
		std::cerr << usageStr << std::endl;
		return EXIT_FAILURE;
	}

	int n		= std::atoi (argv[1]);
	int p		= std::atoi (argv[2]);
	int seed	= (argn==4) ? std::atoi (argv[3]) : -1;

	if (n%p != 0) {
		std::cerr << "The length of vector must be multiple of number of processors!\n";
		return EXIT_FAILURE;
	}

	//std::cout << "Input args ==>   n: " << n << "\tp: " << p << "\tseed: " << seed << std::endl;

	BSP<int> tsikinAlgorithm;
	IntVector unorderedVector;
	IntVector orderedVector;
	std::vector<IntVector> bspInput;
	std::vector<IntVector> bspOutput;

	setupBsp (std::ref(tsikinAlgorithm), n, p, seed, std::ref(unorderedVector), std::ref(bspInput), std::ref(bspOutput));

	tsikinAlgorithm.runAndWait (std::ref(bspInput), std::ref(bspOutput));

	std::cout << "Output vector:\n";
	for (auto out : bspOutput) {
		orderedVector.insert (orderedVector.end(), out.begin(), out.end());
		/*for (auto el : out) {
			std::cout << el << " ";
		}*/
	}
	for (auto el : orderedVector)
		std::cout << el << " ";
	std::cout << std::endl << std::endl;

	std::cout << "Sorted?  " << (std::is_sorted (orderedVector.begin(), orderedVector.end()) ? "YES!\n" : "NO!\n");



	return EXIT_SUCCESS;
}