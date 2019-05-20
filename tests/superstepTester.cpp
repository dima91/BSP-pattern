
#include <superstep.hpp>

#include <iostream>
#include <future>
#include <algorithm>

using namespace std;


vector<int> s0Function (vector<int>& v) {
	std::for_each (v.begin(), v.end(), [] (int &i) {
		cout << ".." << i << endl;
	});

	return v;
}



int main (int argn, char **argv) {
	cout << "Hello user!!\n";

	std::vector<std::vector<int>> comProto;
	Superstep<int> s0 (s0Function, 3, comProto);

	return 0;
}