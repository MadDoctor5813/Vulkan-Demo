#pragma once

#include <GLFW\glfw3.h>

class App {

public:
	App();
	~App();

	void runLoop();

private:
	GLFWwindow * window;

	const int WIDTH = 720;
	const int HEIGHT = 480;

	void initGLFW();
	void initVulkan();

};

