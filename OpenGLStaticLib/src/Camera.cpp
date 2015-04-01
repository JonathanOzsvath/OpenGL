#include <Camera.h>


Camera::Camera(float width, float height, float movingArea)
{
	firstMouse = true;
	cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	yaw = -90.0f;
	pitch = 0.0f;
	lastX = width / 2.0;
	lastY = height / 2.0;
	deltaTime = 0.0f;
	lastFrame = 0.0f;

}

void Camera::setView()
{
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

mat4 Camera::getView()
{
	return view;
}