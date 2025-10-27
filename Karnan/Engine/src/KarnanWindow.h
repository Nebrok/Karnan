#pragma once
#include "GLFW/glfw3.h"


class KarnanWindow
{
private:
	GLFWwindow* _window;

	int _width;
	int _height;

public:
	KarnanWindow(int width, int height);
	~KarnanWindow();

	GLFWwindow* GetWindowReference() { return _window; };


private:



};