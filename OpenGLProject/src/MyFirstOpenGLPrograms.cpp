#include "GLShader.h"
#include "GLBuffer.h"
#include "GLProgram.h"
#include "GLFWProgram.h"
#include "vbocube.h"
#include "vboplane.h"
#include "Cube.h"
#include "CubeMapTexture.h"
#include "Texture.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtx\rotate_vector.hpp>
#include "tgaio.h"
#include <string>

using namespace glm;
using namespace std;

GLFWwindow *window;
GLProgram envProg;
GLuint width = 1920, height = 1080;

vec4 worldLight = vec4(0.0f, 0.0f, 5.0f, 1.0f);

mat4 model;
mat4 projection;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
Camera camera(width, height, 25.0f);
Cube *cube, *smallCube;
GLfloat area = 50.0f, sArea = 10.0f;

Texture *smallCubeTexture;

VBOPlane *plane;

vec3 smallCubePosition;

void compileShader()
{
	GLShader envVert(GLShader::GLShaderType::VERTEX);
	envVert.readShader("src/shader/spot.vs");
	envVert.compileShader();

	GLShader envFrag(GLShader::GLShaderType::FRAGMENT);
	envFrag.readShader("src/shader/spot.fs");
	envFrag.compileShader();

	envProg.setShaders({ envVert.getId(), envFrag.getId() });
	envProg.link();
}

void envSetMatrices()
{
	mat4 mv = camera.getView() * model;
	envProg.setUniform("ModelViewMatrix", mv);
	envProg.setUniform("ModelMatrix", model);
	envProg.setUniform("NormalMatrix",
		mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	envProg.setUniform("MVP", projection * mv);
}

void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 300.0f);
}

void init()
{
	compileShader();
	glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	//cube = new Cube(area, false);
	smallCube = new Cube(sArea,true);
	plane = new VBOPlane(50.0f, 50.0f, 1, 1);
	envProg.use();
	
	for (int i = 0; i < 4; i++)
	{
		string str = "Spot[" + to_string(i) + "].intensity";
		const char *tmpPosition = str.c_str();
		envProg.setUniform(tmpPosition, vec3(1.0f, 1.0f, 1.0f));
		str = "Spot[" + to_string(i) + "].exponent";
		tmpPosition = str.c_str();
		envProg.setUniform(tmpPosition, 30.0f);
		str = "Spot[" + to_string(i) + "].cutoff";
		tmpPosition = str.c_str();
		envProg.setUniform(tmpPosition, 15.0f);
	}
	/*envProg.setUniform("Spot[0].intensity", vec3(1.0f, 1.0f, 1.0f));
	envProg.setUniform("Spot[0].exponent", 30.0f);
	envProg.setUniform("Spot[0].cutoff", 15.0f);
	envProg.setUniform("Spot[1].intensity", vec3(1.0f, 1.0f, 1.0f));
	envProg.setUniform("Spot[1].exponent", 30.0f);
	envProg.setUniform("Spot[1].cutoff", 15.0f);*/
	
	CubeMapTexture cubetexture("src/texture/brick/brick", 1024);
	smallCubeTexture = new Texture("src/texture/flower.tga", 0);

	smallCubePosition = vec3(0.0f, sArea / 2.0f, 0.0f);
}

void mainloop()
{
	while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		camera.do_movement(deltaTime);

		vec4 lightPos[4];
		lightPos[0]= vec4(area / 2.0f, area / 2.0f, area / 2.0f, 1.0f);
		lightPos[1] = vec4(area / 2.0f, area / 2.0f, -area / 2.0f, 1.0f);
		lightPos[2] = vec4(-area / 2.0f, area / 2.0f, -area / 2.0f, 1.0f);
		lightPos[3] = vec4(-area / 2.0f, area / 2.0f, area / 2.0f, 1.0f);

		mat3 normalMatrix = mat3(vec3(camera.getView()[0]), vec3(camera.getView()[1]), vec3(camera.getView()[2]));
		for (int i = 0; i < 4; i++)
		{
			string str = "Spot[" + to_string(i) + "].position";
			const char *tmpPosition = str.c_str();
			envProg.setUniform(tmpPosition, camera.getView() * lightPos[i]);
			str = "Spot[" + to_string(i) + "].direction";
			const char *tmpDirection = str.c_str();
			envProg.setUniform(tmpDirection, normalMatrix * (smallCubePosition - vec3(lightPos[i])));
		}

		envProg.setUniform("Kd", 0.9f, 0.1f, 0.3f);
		envProg.setUniform("Ks", 0.95f, 0.95f, 0.95f);
		envProg.setUniform("Ka", 0.4f * 0.3f, 0.5f * 0.3f, 0.3f * 0.3f);
		envProg.setUniform("Shininess", 100.0f);

		model = mat4(1.0f);
		model *= translate(smallCubePosition);
		envSetMatrices();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, smallCubeTexture->id);
		smallCube->render();

		envProg.setUniform("Kd", 0.9f, 0.5f, 0.3f);
		envProg.setUniform("Ks", 0.95f, 0.95f, 0.95f);
		envProg.setUniform("Ka", 1.0f,0.0f,0.0f);
		envProg.setUniform("Shininess", 100.0f);

		model = mat4(1.0f);
		envSetMatrices();
		plane->render();
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void keyFunction(GLFWwindow *window, int key, int scanCode, int action, int mods)
{
	GLfloat cubeSpeed = 0.2f;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	camera.setKey(key, action, deltaTime);

	if (action == GLFW_REPEAT || action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_UP:
			if (smallCubePosition.y < (area / 2.0f) - sArea / 2.0f)
				smallCubePosition += vec3(0.0f, cubeSpeed, 0.0f);
			break;
		case GLFW_KEY_DOWN:
			if (smallCubePosition.y > -(area / 2.0f) + sArea / 2.0f)
				smallCubePosition -= vec3(0.0f, cubeSpeed, 0.0f);
			break;
		case GLFW_KEY_RIGHT:
			if (smallCubePosition.x < (area / 2.0f) - sArea / 2.0f)
				smallCubePosition += vec3(cubeSpeed, 0.0f, 0.0f);
			break;
		case GLFW_KEY_LEFT:
			if (smallCubePosition.x > -(area / 2.0f) + sArea / 2.0f)
				smallCubePosition -= vec3(cubeSpeed, 0.0f, 0.0f);
			break;
		case GLFW_KEY_M:
			if (smallCubePosition.z < (area / 2.0f) - sArea / 2.0f)
				smallCubePosition += vec3(0.0f, 0.0f, cubeSpeed);
			break;
		case GLFW_KEY_N:
			if (smallCubePosition.z > -(area / 2.0f) + sArea / 2.0f)
				smallCubePosition -= vec3(0.0f, 0.0f, cubeSpeed);
			break;
		default:
			break;
		}
	}

	glfwPollEvents();
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	camera.setMouse(xpos, ypos);
}

int main()
{
	window = initWindow("StrandBall", width, height);

	init();
	resize(width, height);

	glfwSetKeyCallback(window, keyFunction);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	mainloop();

	return 0;
}