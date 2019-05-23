
#include <bsp.hpp>

#include <iostream>



int main (int argn, char **argv) {
	std::cout << "Hello user!\n";

	BSP<int> bspPattern;

	std::shared_ptr<Superstep<int>> s0;
	bspPattern.addSuperstep (s0);

	//bspPattern.addSuperstep (s0);
	//bspPattern.addSuperStep (s1);
	//bspPattern.addSuperStep (s2);

	return 0;
}