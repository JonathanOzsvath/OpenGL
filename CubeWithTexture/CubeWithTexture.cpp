#include "GLShader.h"
#include "GLBuffer.h"
#include "GLProgram.h"
#include "GLFWProgram.h"
#include "vbosphere.h"
#include "Cube.h"
#include "CubeMapTexture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm\gtx\transform.hpp>
#include "tgaio.h"

using namespace glm;
using namespace std;

GLFWwindow *window;
GLProgram envProg;
GLProgram sphereProg;
GLuint width = 1280, height = 720;

vec4 worldLight = vec4(5.0f, 1.0f, 10.0f, 1.0f);
vec3 camPos = vec3(0.0f, 0.01f, 10.0f);
vec3 camLook = vec3(0.0f, 0.0f, 0.0f);
vec3 up = vec3(0.0f, 0.0f, 1.0f);

mat4 model;
mat4 view;
mat4 projection;

Cube *cube;
VBOSphere *sphere;

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
	sphereVert.readShader("src/shader/sphere.vert");
	sphereVert.compileShader();

	GLShader sphereFrag(GLShader::GLShaderType::FRAGMENT);
	sphereFrag.readShader("src/shader/sphere.frag");
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

	view = glm::lookAt(camPos, camLook, up);

	envProg.setUniform("LightPosition", view * vec4(0.0f, 0.0f, 0.0f, 1.0f));
	envProg.setUniform("Kd", vec3(0.9f, 0.5f, 0.3f));
	envProg.setUniform("Ld", vec3(1.0f, 1.0f, 1.0f));

	sphereProg.setUniform("LightPosition", view * worldLight);

	CubeMapTexture texture("src/texture/cubemap_night/night", 256);
}

void mainloop()
{
	while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		model = mat4(1.0f);
		model *= rotate(radians(180.0f), vec3(1.0f, 0.0f, 0.0f));

		envProg.use();
		envSetMatrices();
		envProg.setUniform("WorldCameraPosition", view);
		envProg.setUniform("DrawSkyBox", true);
		cube->render();

		model = mat4(1.0f);
		sphereProg.use();
		glm::mat4 mv = view * model;
		sphereSetMatrices();

		sphereProg.setUniform("Ld", vec3(1.0f, 1.0f, 1.0f));
		sphereProg.setUniform("Kd", vec3(1.0f, 1.0f, 0.0f));

		sphere->render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void keyFunction(GLFWwindow *window, int key, int scanCode, int action, int mods)
{

	switch (key)
	{
	case GLFW_KEY_UP:
		view = glm::rotate(view, -0.1f, glm::vec3(1.0, 0.0, 0.0));
		break;
	case GLFW_KEY_DOWN:
		view = glm::rotate(view, 0.1f, glm::vec3(1.0, 0.0, 0.0));
		break;
	case GLFW_KEY_LEFT:
		view = glm::rotate(view, 0.1f, glm::vec3(0.0, 0.0, 1.0));
		break;
	case GLFW_KEY_RIGHT:
		view = glm::rotate(view, -0.1f, glm::vec3(0.0, 0.0, 1.0));
		break;
	case GLFW_KEY_S:
		view = glm::translate(view, glm::vec3(0.0, 0.1, 0.0));
		break;
	case GLFW_KEY_A:
		view = glm::translate(view, glm::vec3(0.1, 0.0, 0.0));

		break;
	case GLFW_KEY_D:
		view = glm::translate(view, glm::vec3(-0.1, 0.0, 0.1));
		//move camera
		break;
	case GLFW_KEY_W:
		view = glm::translate(view, glm::vec3(0.0, -0.1, 0.0));
		//move camera
		break;
	default:
		printf("Bad key :(\n");
		break;
	}

	glfwPollEvents();
}

int main()
{
	window = initWindow("CubeWithTexture", width, height);

	init();
	resize(width, height);

	glfwSetKeyCallback(window, keyFunction);

	mainloop();

	return 0;
}