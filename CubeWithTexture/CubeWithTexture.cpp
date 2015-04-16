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
mat4 projection;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
Camera camera(width, height, 25.0f);
Cube *cube;
VBOSphere *sphere,*sphere2;
Texture *texture, *texture2, *texture3, *texture4;

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
	mat4 mv = camera.getView() * model;
	envProg.setUniform("ModelMatrix", model);
	envProg.setUniform("MVP", projection * mv);
}

void sphereSetMatrices()
{
	glm::mat4 mv = camera.getView() * model;
		sphereProg.setUniform("ModelViewMatrix", mv);
		sphereProg.setUniform("NormalMatrix", mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
		sphereProg.setUniform("MVP", projection * camera.getView() * model);
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

	envProg.use();
	envProg.setUniform("LightPosition", camera.getView() * vec4(0.0f, 0.0f, 0.0f, 1.0f));
	envProg.setUniform("Kd", vec3(0.9f, 0.5f, 0.3f));
	envProg.setUniform("Ld", vec3(1.0f, 1.0f, 1.0f));

	CubeMapTexture cubetexture("src/texture/citadella/night", 2048);
	//CubeMapTexture cubeTexture("src/texture/cubemap_night/night", 256);
	//CubeMapTexture cubetexture("src/texture/brick/brick", 1024);

	sphereProg.use();
	sphereProg.setUniform("Light.Intensity", vec3(0.7f, 0.7f, 0.7f));

	texture = new Texture("src/texture/eltrusko.tga", 0);
	texture2 = new Texture("src/texture/jabulani.tga", 1);
	texture3 = new Texture("src/texture/Teamgeist.tga", 2);
	texture4 = new Texture("src/texture/europass.tga", 2);
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

		model = mat4(1.0f);
		model *= rotate(radians(180.0f), vec3(1.0f, 0.0f, 0.0f));

		envProg.use();
		envSetMatrices();
		envProg.setUniform("WorldCameraPosition", vec3(50.0f,50.0f,50.0f));
		envProg.setUniform("DrawSkyBox", true);
		cube->render();

		model = mat4(1.0f);
		sphereProg.use();
		sphereSetMatrices();

		sphereProg.setUniform("Light.Position", vec4(10.0f, 10.0f, 10.0f, 0.5f));
		sphereProg.setUniform("Material.Kd", 0.9f, 0.9f, 0.9f);
		sphereProg.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
		sphereProg.setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
		sphereProg.setUniform("Material.Shininess", 50.0f);

		
		glBindTexture(GL_TEXTURE_2D, texture->id);
		sphere->render();

		model = mat4(1.0);
		model *= translate(vec3(10.0f, 0.0f, 10.0f));
		sphereSetMatrices();
		glBindTexture(GL_TEXTURE_2D, texture2->id);
		sphere2->render();

		model = mat4(1.0);
		model *= translate(vec3(0.0f, 5.0f, 10.0f));
		sphereSetMatrices();
		glBindTexture(GL_TEXTURE_2D, texture3->id);
		sphere2->render();

		model = mat4(1.0);
		model *= translate(vec3(0.0f, 15.0f, 10.0f));
		sphereSetMatrices();
		glBindTexture(GL_TEXTURE_2D, texture4->id);
		sphere2->render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}


void keyFunction(GLFWwindow *window, int key, int scanCode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	camera.setKey(key, action, deltaTime);

	glfwPollEvents();
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	camera.setMouse(xpos, ypos);
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