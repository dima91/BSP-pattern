/**
 * \file tiskin.cpp
 * \brief Main file containing the implementation of Tiskin sorting algorithm.
 * \author Luca Di Mauro
 */


#include <bsp.hpp>
#include <uTimer.hpp>

#include <iostream>
#include <random>
#include <map>
#include <mutex>
#include <algorithm>



#ifdef PRINT_FULL
	#define TISKIN_PRINT_V(lbl, vec, footer) {\
		{\
			std::unique_lock<std::mutex> lock (outputMutex);\
			std::cout << lbl << std::flush;\
			for (auto el : vec)\
				std::cout << el << " " << std::flush;\
			std::cout << std::endl << footer;\
		}\
	}
#else
	#define TISKIN_PRINT_V(lbl, vec, footer) {}
#endif


using el_t						= int;
using IntVector					= std::vector<el_t>;
using IntCommunicationProtocol	= Superstep<el_t>::CommunicationProtocol;
using CommunicationProtocolFun	= std::function<IntCommunicationProtocol (IntVector &)>;
using Parameters				= std::tuple<uint, uint, int, bool>;


std::mutex outputMutex;
std::map<uint, IntVector> s0VectorsMap;
std::mutex mapMutex;




bool parseArgs (int argn, char **argv, Parameters &params) {
	//           0        1  2  3  4  5
	// ./tiskinAlgorithm 256 8 -s 10 -a
	// ./tiskinAlgorithm 256 8 -a -s 10
	if (argn < 3 || argn > 6) {
		return false;
	}

	try {
		uint n			= std::atoi (argv[1]);
		uint p			= std::atoi (argv[2]);
		int seed		= -1;
		bool affinity	= false;

		if (n > 1073741824) {		// 2^30 
			char inCh;
			std::cout << "WARNING:\ttoo high number of elements: it may be cause problems\n"
					"Do you want to continue? [y/<anthing>]" << std::flush;
			std::cin >> inCh;
			if (inCh != 'y') {
				return false;
			}
		}
		
		
		if (argn == 4) {
			std::string arg3	= argv[3];
			if (arg3 == "-a")
				affinity	= true;
			else
				return false;
		}
		else if (argn == 5) {
			std::string arg3	= argv[3];
			if (arg3 == "-s")
				seed	= std::atoi (argv[4]);
			else
				return false;
		}
		else if (argn == 6) {
			std::string arg3	= argv[3];
			std::string arg4	= argv[4];
			std::string arg5	= argv[5];

			if (arg3 == "-s" && arg5 == "-a") {
				seed		= std::atoi (argv[4]);
				affinity	= true;
			}
			else if (arg3 == "-a" && arg4 == "-s") {
				seed		= std::atoi (argv[5]);
				affinity	= true;
			}
			else
				return false;
		}

		if (n%p != 0) {
			std::cerr << "The length of vector must be multiple of number of processors!\n";
			return false;
		}

		params	= Parameters (n, p, seed, affinity);

		return true;
	} catch (...) {
		return false;
	}
}




void createRandomVector (IntVector &input, int seed) {
	uint idx	= 0;
	std::iota (input.begin(), input.end(), idx++);
	
	std::mt19937 engine (seed);
	std::default_random_engine engine1 {};

	std::shuffle (std::begin (input), std::end(input), engine);
}




void findOutSeparators (IntVector &target, IntVector &source, uint n) {
	auto it		= source.begin ();
	uint size1	= source.size ()-1;		// Number of elements to be taken into account (belonging to vector)
	uint n1		= n-1;					// Number of elements to be selected
	uint h		= size1/n1;				// Size of subset
    uint k		= size1%n1;				// Number of subset with size h+1
	uint i		= 0;					// Index of target array
	uint tmpN1	= 0;
	
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
}




void setupBsp (BSP<el_t> &tAlg, uint n, uint p, int seed, IntVector &input,
				std::vector<IntVector> &bspInputs, std::vector<IntVector> &bspOutputs) {
	
	std::random_device randomDevice;
	input	= IntVector	(n);
	
	{
		UTimer randomVector ("Creating random vector");
		createRandomVector (input, (seed==-1) ? randomDevice() : seed);
	}


	{
		UTimer bspInputVectors ("Creating BSP input vectors");
		
		bspInputs	= std::vector<IntVector> (p);
		bspOutputs	= std::vector<IntVector> (p);

		uint actInputLen	= n/p;
		for (uint i=0; i<p; i++) {
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

	BSP<el_t>::SuperstepPointer s0	= BSP<el_t>::SuperstepPointer (new Superstep<el_t> (0));

	for (uint i= 0; i<p; i++) {
		s0->addActivity (
			[] (uint activityIndex, IntVector &actInput) {
				TISKIN_PRINT_V ("Input vector of activity " << activityIndex << ": ", actInput, "");
				std::sort (actInput.begin(), actInput.end());
				mapMutex.lock ();
				s0VectorsMap[activityIndex]	= actInput;
				mapMutex.unlock ();
			},
			[p] (uint activityIndex, IntVector &elements) {
				//TISKIN_PRINT_V ("Input vector of activity " << activityIndex << ": ", actInput, "");
				IntCommunicationProtocol cp (p);
				cp[0]	= IntVector (p+1);
				findOutSeparators (std::ref(cp[0]), std::ref(elements), p+1);
				for (uint i=1; i<p; i++) {
					cp[i]	= IntVector (p+1);
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

	BSP<el_t>::SuperstepPointer s1	= BSP<el_t>::SuperstepPointer (new Superstep<el_t> (1));

	for (uint i= 0; i<p; i++) {
		s1->addActivity (
			[] (uint activityIndex, IntVector &actInput) {
				TISKIN_PRINT_V ("Input vector of activity " << activityIndex << ": ", actInput, "");
				std::sort (actInput.begin(), actInput.end());
			},
			([p] (uint activityIndex, IntVector &commElements) {
				//TISKIN_PRINT_V ("Input vector of activity " << activityIndex << ": ", actInput, "");
				std::vector<IntVector> cp (p);
				IntVector separators (p+1);
				findOutSeparators (std::ref(separators), std::ref(commElements), p+1);
				
				mapMutex.lock ();
				IntVector &pi	= s0VectorsMap[activityIndex];
				mapMutex.unlock ();
				size_t i				= 0;	// Index for 'pi' vector
				size_t j				= 1;	// Index for 'separators' vector
				size_t k				= 0;	// Index for 'cp' vector

				while (i < pi.size()) {
					cp[k]	= IntVector ();
					while (i < pi.size() && pi[i] <= separators[j]) {
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

	BSP<el_t>::SuperstepPointer s2	= BSP<el_t>::SuperstepPointer (new Superstep<el_t> (2));

	for (uint i= 0; i<p; i++) {
		s2->addActivity (
			[] (uint activityIndex, IntVector &actInput) {
				TISKIN_PRINT_V ("Input vector of activity " << activityIndex << ": ", actInput, "");
				std::sort (actInput.begin(), actInput.end());
			},
			([p] (uint activityIndex, IntVector &elements) {
				std::vector<IntVector> cp (p);
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
	std::string usageStr	= 	"Usage: ./bin/TiskinAlgorithm <n> <p> [-s seed] [-a]"
								"\n\t\tn\t\tNumber of items contained in the vector to be ordered"
								"\n\t\tp\t\tNumber of processor (parallel activities)"
								"\n\t\t-s seed\t\tSeed to be given as arguments to generate random numbers"
								"\n\t\t-a\t\tTry to set affinity for worker threads";

	Parameters parameters;
	
	if (!parseArgs (argn, argv, std::ref (parameters))) {
		std::cerr << usageStr << std::endl;
		return EXIT_FAILURE;
	}


	uint n			= std::get<0> (parameters);
	uint p			= std::get<1> (parameters);
	int seed		= std::get<2> (parameters);
	bool affinity	= std::get<3> (parameters);

	std::cout << "Using a vector of   size " << n << "   and   "  << p << " processors,  "
					" with   seed=" << seed << "   and   affinity=" << affinity << std::endl << std::endl;

	BSP<el_t> tiskinAlgorithm;
	IntVector cppUnorderedVector;
	IntVector unorderedVector;
	IntVector orderedVector;
	std::vector<IntVector> bspInput;
	std::vector<IntVector> bspOutput;

	{;
		std::cout << std::endl <<
		 			"======================\n" <<
					"Setting up environment\n";
		UTimer environmentTimer ("Environment setup");
		setupBsp (std::ref(tiskinAlgorithm), n, p, seed, std::ref(unorderedVector), std::ref(bspInput), std::ref(bspOutput));	
		#ifdef CPP_UNORDERED_VECTOR
		cppUnorderedVector	= unorderedVector;
		#endif
	}

	TISKIN_PRINT_V ("Algorithm's input vector:\n", unorderedVector, "\n");

	{
		std::cout << std::endl <<
				 	"====================\n" <<
					"Starting computation\n";
		UTimer computationTimer ("Whole algorithm");
		tiskinAlgorithm.runAndWait (std::ref(bspInput), std::ref(bspOutput), affinity);
		std::cout << std::endl << std::endl;
	}

	for (auto out : bspOutput) {
		orderedVector.insert (orderedVector.end(), out.begin(), out.end());
	}
	TISKIN_PRINT_V ("Algorithm's output vector:\n", orderedVector, "\n");
	

	{
		UTimer timer ("Check of effectively sorted vector");
		if (!std::is_sorted (orderedVector.begin(), orderedVector.end()))
			std::cout << "The vector is NOT sorted!\nVERY DANGEROUS!\n";
	}


	#ifdef COMPUTE_SEQUENTIAL
	try {
		std::cout << std::endl <<
				 	"=====================\n" <<
					"C++ sorting algorithm\n";
		UTimer cppTimer ("C++ sort algorithm");
		std::sort (cppUnorderedVector.begin(), cppUnorderedVector.end());
	} catch (std::runtime_error &e) {
		std::cerr << "Catched an exception! " << e.what() << std::endl;
	}
	#endif

	std::cout << std::endl ;


	return EXIT_SUCCESS;
}