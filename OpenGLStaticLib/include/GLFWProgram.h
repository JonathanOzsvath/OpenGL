#pragma once

#define GLFW_PROGRAM

#include "cookbookogl.h"
#include <GLFW\glfw3.h>
#include <string>


GLFWwindow* initWindow(std::string s,int width,int height)
{
    if (!glfwInit()) exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    GLFWwindow* window;
    // Open the window
    std::string title = s;
    //window = glfwCreateWindow(width, height, title.c_str(), glfwGetPrimaryMonitor(), NULL);
	window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);


    if (ogl_LoadFunctions() == ogl_LOAD_FAILED)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    return window;
}