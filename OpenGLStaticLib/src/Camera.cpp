#include <Camera.h>


Camera::Camera(float width, float height, float movingArea)
{
	cornel = false;
	firstMouse = true;
	cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	yaw = -90.0f;
	pitch = 0.0f;
	lastX = width / 2.0;
	lastY = height / 2.0;
	this->movingArea = movingArea;
	setView();
	setSpherePosition();
}

void Camera::setView()
{
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

mat4 Camera::getView()
{
	return view;
}

void Camera::do_movement(float deltaTime)
{
	if (!cornel)
	{
		GLfloat cameraSpeed = 10.0f * deltaTime;
		if (keys[GLFW_KEY_W])
		{
			vec3 tmp = cameraPos + (cameraFront*cameraSpeed);
			if (abs(tmp.x) < movingArea-5 && abs(tmp.y) < movingArea-5 && abs(tmp.z) < movingArea-5)
			{
				cameraPos += cameraSpeed * cameraFront;
				setView();
				setSpherePosition();
			}
		}
		if (keys[GLFW_KEY_S])
		{
			vec3 tmp = cameraPos - (cameraFront*cameraSpeed);
			if (abs(tmp.x) < movingArea - 5 && abs(tmp.y) < movingArea - 5 && abs(tmp.z) < movingArea - 5)
			{
				cameraPos -= cameraSpeed * cameraFront;
				setView();
				setSpherePosition();
			}
		}
		if (keys[GLFW_KEY_A])
		{
			vec3 tmp = cameraPos - (cameraFront*cameraSpeed);
			if (abs(tmp.x) < movingArea - 5 && abs(tmp.y) < movingArea - 5 && abs(tmp.z) < movingArea - 5)
			{
				cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
				setView();
				setSpherePosition();
			}
		}
		if (keys[GLFW_KEY_D])
		{
			vec3 tmp = cameraPos + (cameraFront*cameraSpeed);
			if (abs(tmp.x) < movingArea - 5 && abs(tmp.y) < movingArea - 5 && abs(tmp.z) < movingArea - 5)
			{
				cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
				setView();
				setSpherePosition();
			}
		}
	}
}


void Camera::setKey(int key, int action, float deltaTime)
{
	GLfloat cameraSpeed = 10.0f * deltaTime;
	if (key ==GLFW_KEY_C && action == GLFW_PRESS)
	{
		if (!cornel)
		{
			tmpCameraPos = cameraPos;
			tmpCameraFront = cameraFront;
			tmpCameraUp = cameraUp;
			cameraPos = vec3(movingArea, movingArea, movingArea);
			cameraFront = vec3(0.0f - cameraPos.x, 0.0f - cameraPos.y, 0.0f - cameraPos.z);
			cameraUp = vec3(0.0f, 1.0f, 0.0f);
			setView();
			cornel = true;
		}
		else
		{
			cornel = false;
			cameraPos = tmpCameraPos;
			cameraFront = tmpCameraFront;
			cameraUp = tmpCameraUp;
			setView();
			setSpherePosition();
		}
	}
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void Camera::setMouse(double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	GLfloat sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
	setView();
}

void Camera::setCornel(bool cornel)
{
	this->cornel = cornel;
}

void Camera::setSpherePosition()
{
	spherePosition = cameraPos;
}

vec3 Camera::getSpherePosition()
{
	return spherePosition;
}