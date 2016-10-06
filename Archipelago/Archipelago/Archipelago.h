#pragma once

//OpenGL
void initGLFWHints();

// Window
GLFWwindow* getWindowInstance();

// Keyboard
//////////////////////////////////////////////////////////////////////////
void error_callback(int error, const char* description);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

