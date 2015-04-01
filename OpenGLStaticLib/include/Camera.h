#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm\gtx\transform.hpp>
#include "cookbookogl.h"
#include <GLFW\glfw3.h>

using namespace glm;
using namespace std;

class Camera
{
public:
	Camera(float, float, float);

	void setView();
	mat4 getView();

private:
	bool keys[1024];
	bool firstMouse;
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	GLfloat yaw;
	GLfloat pitch;
	GLfloat lastX;
	GLfloat lastY;
	GLfloat deltaTime;
	GLfloat lastFrame;
	GLfloat movingArea;

	mat4 view;
};