#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "App.h"

int main() {
	App app;
	app.runLoop();
	return 0;
}