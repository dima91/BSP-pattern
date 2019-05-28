/**
 * \file tsikin.cpp
 * \brief Main file containing the implementation of Tsikin sorting algorithm.
 * \author Luca Di Mauro
 */


#include <bsp.hpp>
#include <uTimer.hpp>

#include <iostream>
#include <random>
#include <map>
#include <mutex>
#include <algorithm>


#define CPP_UNORDERED_VECTOR


//#define SHARED_PTR
#ifdef SHARED_PTR
	using el_t	= std::shared_ptr<int>;
#else
	using el_t	= int;
#endif


//#define DEBUG
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


using IntVector					= std::vector<el_t>;
using IntCommunicationProtocol	= Superstep<el_t>::CommunicationProtocol;
using CommunicationProtocolFun	= std::function<IntCommunicationProtocol (IntVector &)>;
using Parameters				= std::tuple<int, int, int, bool>;


std::mutex outputMutex;
std::map<int, IntVector> s0VectorsMap;
std::mutex mapMutex;




bool parseArgs (int argn, char **argv, Parameters &params) {
	//           0        1  2  3  4  5
	// ./tsikinAlgorithm 256 8 -s 10 -a
	// ./tsikinAlgorithm 256 8 -a -s 10
	if (argn < 3 || argn > 6) {
		return false;
	}

	try {
		int n			= std::atoi (argv[1]);
		int p			= std::atoi (argv[2]);
		int seed		= -1;
		bool affinity	= false;
		
		
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




void createRandomVector (IntVector &input, int seed, int parDeg) {
#if 1
	auto threadFun	= [&input] (int startIdx, int endIdx, int startInt) {
		//std::cout << "Filling vector from " << startIdx << " to " << endIdx << " with values from " << startInt << std::endl;
		while (startIdx!=endIdx) {
			input[startIdx]	= startInt++;
			startIdx++;
		}
	};

	std::vector<std::thread> threadV (4);
	int offset		= input.size() / threadV.size();
	int startInt	= 0;
	for (size_t i=0; i<threadV.size(); i++) {
		threadV[i]	= std::thread (threadFun, (i*offset), ((i+1)*offset), startInt+(i*offset));
	}

	for (auto &t : threadV)
		t.join ();
#else
	int idx	= 1966;
	std::iota (input.begin(), input.end(), idx++);
#endif

	std::random_shuffle (input.begin(), input.end());
}



/*void createRandomVector (IntVector &input, int seed, int parDeg) {
	std::mt19937 mt (seed);
	std::uniform_int_distribution<int> dist (1, input.size()*10);
	int nextInt	= 0;

	for (size_t i=0; i<input.size(); i++) {
		do {
			nextInt	= dist(mt);
		} while (std::find(input.begin(), input.end(), nextInt) != input.end());
		input[i]	= nextInt;
	}
}*/




/*void createRandomVector (IntVector &input, int seed, int parDeg) {
	int idx			= 0;
	int inputSize	= input.size ();
	std::list<int> sourceList (inputSize*10);
	std::mt19937 mt (seed);
	std::uniform_int_distribution<int> dist (1, inputSize*10);

	std::iota (sourceList.begin(), sourceList.end(), idx++);

	for (size_t i=0; i<input.size(); i++) {
		idx			= dist(mt);
		auto it		= sourceList.begin ();
		std::advance (it, idx);
		input[i]	= *it;
		sourceList.erase (it);
	}
}*/


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


void setupBsp (BSP<el_t> &tAlg, int n, int p, int seed, IntVector &input,
				std::vector<IntVector> &bspInputs, std::vector<IntVector> &bspOutputs) {
	
	std::random_device randomDevice;
	input	= IntVector	(n);
	
	{
		UTimer randomVector ("Creating random vector");
		createRandomVector (input, (seed==-1) ? randomDevice() : seed, p);
	}
	TSIKIN_PRINT_V ("Input vector", input, "\n");


	{
		UTimer bspInputVectors ("Creating BSP input vectors");
		
		bspInputs	= std::vector<IntVector> (p);
		bspOutputs	= std::vector<IntVector> (p);

		int actInputLen	= n/p;
		for (int i=0; i<p; i++) {
			bspInputs[i]	= IntVector (actInputLen);
			auto &vI		= bspInputs[i];
			for (int j=0; j<actInputLen; j++) {
				vI[j]	= input[(actInputLen*i)+j];
			}

			//TSIKIN_PRINT_V ("Proc " << i << " input", bspInputs[i], "");
		}
	}


	UTimer superstepCreator ("Creating supersteps");


	// ============================================================
	// ============================================================
	// Superstep 0

	BSP<el_t>::SuperstepPointer s0	= BSP<el_t>::SuperstepPointer (new Superstep<el_t> ());

	for (int i= 0; i<p; i++) {
		s0->addActivity (
			[] (int activityIndex, IntVector &actInput) {
				std::sort (actInput.begin(), actInput.end());
				s0VectorsMap[activityIndex]	= actInput;
			},
			[p] (int activityIndex, IntVector &elements) {
				IntCommunicationProtocol cp (p);
				cp[0]	= IntVector (p+1);
				findOutSeparators (std::ref(cp[0]), std::ref(elements), p+1);
				for (int i=1; i<p; i++) {
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

	BSP<el_t>::SuperstepPointer s1	= BSP<el_t>::SuperstepPointer (new Superstep<el_t> ());

	for (int i= 0; i<p; i++) {
		s1->addActivity (
			[] (int activityIndex, IntVector &actInput) {
				//TSIKIN_PRINT_V ("Input " << activityIndex, actInput, "");
				std::sort (actInput.begin(), actInput.end());
				//TSIKIN_PRINT_V ("Output " << activityIndex, actInput, "");
			},
			([p] (int activityIndex, IntVector &commElements) {
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

	BSP<el_t>::SuperstepPointer s2	= BSP<el_t>::SuperstepPointer (new Superstep<el_t> ());

	for (int i= 0; i<p; i++) {
		s2->addActivity (
			[] (int activityIndex, IntVector &actInput) {
				std::sort (actInput.begin(), actInput.end());
			},
			([p] (int activityIndex, IntVector &elements) {
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
	std::string usageStr	= 	"Usage: ./bin/TsikinAlgorithm <n> <p> [-s seed] [-a]"
								"\n\t\tn\t\tNumber of items contained in the vector to be ordered"
								"\n\t\tp\t\tNumber of processor (parallel activities)"
								"\n\t\t-s seed\t\tSeed to be given as arguments to generate random numbers"
								"\n\t\t-a\t\tTry to set affinity for worker threads";

	Parameters parameters;

	
	if (!parseArgs (argn, argv, std::ref (parameters))) {
		std::cerr << usageStr << std::endl;
		return EXIT_FAILURE;
	}


	int n			= std::get<0> (parameters);
	int p			= std::get<1> (parameters);
	int seed		= std::get<2> (parameters);
	bool affinity	= std::get<3> (parameters);

	std::cout << "Using a vector of   size " << n << "   and   "  << p << " processors,  "
					" with   seed=" << seed << "   and   affinity=" << affinity << std::endl << std::endl;

	BSP<el_t> tsikinAlgorithm;
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
		setupBsp (std::ref(tsikinAlgorithm), n, p, seed, std::ref(unorderedVector), std::ref(bspInput), std::ref(bspOutput));	
		#ifdef CPP_UNORDERED_VECTOR
		cppUnorderedVector	= unorderedVector;
		#endif
	}

	{
		std::cout << std::endl <<
				 	"====================\n" <<
					"Starting computation\n";
		UTimer computationTimer ("Whole algorithm");
		tsikinAlgorithm.runAndWait (std::ref(bspInput), std::ref(bspOutput), affinity);
		std::cout << std::endl << std::endl;
	}

	for (auto out : bspOutput) {
		orderedVector.insert (orderedVector.end(), out.begin(), out.end());
	}
	std::cout << "\nSorted?  " << ((std::is_sorted (orderedVector.begin(), orderedVector.end())) ? "YES" : "NO") << std::endl;
	TSIKIN_PRINT_V ("Output vector", orderedVector, "");


	#ifdef CPP_UNORDERED_VECTOR
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

	std::cout << "Bye!\n";


	return EXIT_SUCCESS;
}