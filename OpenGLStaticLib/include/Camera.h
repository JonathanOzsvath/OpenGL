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

	void setCameraPos(vec3);
	vec3 getCameraPos();
	void setCameraFront(vec3);
	vec3 getCameraFront();
	void setCameraUp(vec3);
	vec3 getCameraUp();
	void setView();
	mat4 getView();
	void do_movement(float);
	void setKey(int, int, float);
	void setMouse(double, double);
	void setSpherePosition();
	vec3 getSpherePosition();

private:
	bool keys[1024];
	bool firstMouse;
	bool cornel;
	glm::vec3 cameraPos;
	glm::vec3 cameraFront;
	glm::vec3 cameraUp;
	glm::vec3 tmpCameraPos;
	glm::vec3 tmpCameraFront;
	glm::vec3 tmpCameraUp;
	GLfloat yaw;
	GLfloat pitch;
	GLfloat lastX;
	GLfloat lastY;
	GLfloat movingArea;

	mat4 view;
	vec3 spherePosition;

	void setCornel(bool);
};