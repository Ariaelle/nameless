#include "app.h"

//std includes
#include <cstdlib>
#include <iostream>


int main() {
	nameless::app app{};
	
	try {
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << "\n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}