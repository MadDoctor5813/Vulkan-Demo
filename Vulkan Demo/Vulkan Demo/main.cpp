#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>

#include "App.h"

int main() {
	App app;
	try {
		app.runLoop();
	}
	catch (const std::runtime_error& e) {
		std::cerr << "Runtime exception: " << e.what() << std::endl;
	}
	return 0;
}