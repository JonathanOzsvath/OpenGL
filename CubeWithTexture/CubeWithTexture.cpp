#include "GLShader.h"
#include "GLBuffer.h"
#include "GLProgram.h"
#include "GLFWProgram.h"
#include "vbosphere.h"
#include "Cube.h"
#include "CubeMapTexture.h"
#include "Texture.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm\gtx\transform.hpp>
#include "tgaio.h"

using namespace glm;
using namespace std;

GLFWwindow *window;
GLProgram envProg;
GLProgram sphereProg;
GLuint width = 1920, height = 1080;

vec4 worldLight = vec4(5.0f, 1.0f, 10.0f, 1.0f);

mat4 model;
mat4 view;
mat4 projection;

//Camera camera(vec3(0.0f, 0.01f, 10.0f), 25.0f);
Cube *cube;
VBOSphere *sphere,*sphere2;
Texture *texture, *texture2;

bool keys[1024];
bool firstMouse = true;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat yaw = -90.0f;	
GLfloat pitch = 0.0f;
GLfloat lastX = width / 2.0;
GLfloat lastY = height / 2.0;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

void setView()
{
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void compileShader()
{
	GLShader envVert(GLShader::GLShaderType::VERTEX);
	envVert.readShader("src/shader/cubemap_reflect.vert");
	envVert.compileShader();

	GLShader envFrag(GLShader::GLShaderType::FRAGMENT);
	envFrag.readShader("src/shader/cubemap_reflect.frag");
	envFrag.compileShader();

	envProg.setShaders({ envVert.getId(), envFrag.getId() });
	envProg.link();

	GLShader sphereVert(GLShader::GLShaderType::VERTEX);
	sphereVert.readShader("src/shader/sphere.vs");
	sphereVert.compileShader();

	GLShader sphereFrag(GLShader::GLShaderType::FRAGMENT);
	sphereFrag.readShader("src/shader/sphere.fs");
	sphereFrag.compileShader();

	sphereProg.setShaders({ sphereVert.getId(), sphereFrag.getId() });
	sphereProg.link();
}

void envSetMatrices()
{
	mat4 mv = view * model;
	envProg.setUniform("ModelMatrix", model);
	envProg.setUniform("MVP", projection * mv);
}

void sphereSetMatrices()
{
	glm::mat4 mv = view * model;
		sphereProg.setUniform("ModelViewMatrix", mv);
		sphereProg.setUniform("NormalMatrix", mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		sphereProg.setUniform("MVP", projection * view * model);
}

void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.05f, 300.0f);
}

void init()
{
	compileShader();
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	cube = new Cube(50.0f, false);
	sphere = new VBOSphere(1.0f, 360, 360);
	sphere2 = new VBOSphere(2.0f, 360, 360);

	//view = camera.getView();
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	envProg.use();
	envProg.setUniform("LightPosition", view * vec4(0.0f, 0.0f, 0.0f, 1.0f));
	envProg.setUniform("Kd", vec3(0.9f, 0.5f, 0.3f));
	envProg.setUniform("Ld", vec3(1.0f, 1.0f, 1.0f));

	//CubeMapTexture cubetexture("src/texture/citadella/night", 2048);
	CubeMapTexture cubeTexture("src/texture/cubemap_night/night", 256);

	sphereProg.use();
	sphereProg.setUniform("Light.Intensity", vec3(1.0f, 1.0f, 1.0f));

	texture = new Texture("src/texture/flower.tga", 0);
	texture2 = new Texture("src/texture/brick1.tga", 1);
}

void do_movement()
{
	// Camera controls
	GLfloat cameraSpeed = 5.0f * deltaTime;
	if (keys[GLFW_KEY_W])
	{
		cameraPos += cameraSpeed * cameraFront;
		setView();
	}
	if (keys[GLFW_KEY_S])
	{
		cameraPos -= cameraSpeed * cameraFront;
		setView();
	}
	if (keys[GLFW_KEY_A])
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		setView();
	}
	if (keys[GLFW_KEY_D])
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		setView();
	}
}

void mainloop()
{
	while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		do_movement();
		/*GLfloat radius = 10.0f;
		GLfloat camX = sin(glfwGetTime()) * radius;
		GLfloat camZ = cos(glfwGetTime()) * radius;
		view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));*/
		
		model = mat4(1.0f);
		model *= rotate(radians(180.0f), vec3(1.0f, 0.0f, 0.0f));

		envProg.use();
		envSetMatrices();
		envProg.setUniform("WorldCameraPosition", view);
		envProg.setUniform("DrawSkyBox", true);
		cube->render();

		model = mat4(1.0f);
		sphereProg.use();
		sphereSetMatrices();

		sphereProg.setUniform("Light.Position", vec4(0.0f, 20.0f, 0.0f, 1.0f));
		sphereProg.setUniform("Material.Kd", 0.9f, 0.9f, 0.9f);
		sphereProg.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
		sphereProg.setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
		sphereProg.setUniform("Material.Shininess", 100.0f);

		
		glBindTexture(GL_TEXTURE_2D, texture->id);

		sphere->render();

		model = mat4(1.0);
		model *= translate(vec3(10.0f, 0.0f, 10.0f));
		sphereSetMatrices();
		glBindTexture(GL_TEXTURE_2D, texture2->id);

		sphere2->render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}


void keyFunction(GLFWwindow *window, int key, int scanCode, int action, int mods)
{
	//view = camera.getKey(key);
	/*GLfloat cameraSpeed = 0.05f;
	if (key == GLFW_KEY_W)
	{
		cameraPos += cameraSpeed * cameraFront;
		setView();
	}
	if (key == GLFW_KEY_S)
	{
		cameraPos -= cameraSpeed * cameraFront;
		setView();
	}
	if (key == GLFW_KEY_A)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		setView();
	}
	if (key == GLFW_KEY_D)
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		setView();
	}*/

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}	

	glfwPollEvents();
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to left
	lastX = xpos;
	lastY = ypos;

	GLfloat sensitivity = 0.05;	// Change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
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

int main()
{
	window = initWindow("CubeWithTexture", width, height);

	init();
	resize(width, height);

	glfwSetKeyCallback(window, keyFunction);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	mainloop();

	return 0;
}