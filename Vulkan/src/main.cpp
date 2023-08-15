#include "graphics/Window.h"

#include <iostream>
#include <stdexcept>
#include <cstdlib>

int main() {
	Window window;
	try {
		window.Initialize();
		window.cleanup();
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	

	return -1;
}