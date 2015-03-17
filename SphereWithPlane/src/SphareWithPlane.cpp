#include <iostream>
#include "GLFWProgram.h"
#include "GLProgram.h"
#include "GLShader.h"
#include "vbocube.h"
#include "vbosphere.h"
#include "vboteapot.h"
#include <glm/gtx/transform.hpp>

using namespace std;

GLFWwindow *window;
GLuint width = 1280, height = 720;
VBOCube *cube;
VBOSphere *sphere;
VBOTeapot *teapot;
GLProgram program;

glm::mat4 lookat;
glm::mat4 projection;
glm::mat4 model;

glm::vec3 moving(0.0f, -7.0f, 6.0f);

void init()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
	glm::mat4 tmp(1.0);
	cube = new VBOCube();
	sphere = new VBOSphere(0.5f, 360, 360);
	teapot = new VBOTeapot(3, tmp);

	GLShader vert(GLShader::GLShaderType::VERTEX);
	vert.readShader("src/shader/plane.vert");
	vert.compileShader();

	GLShader frag(GLShader::GLShaderType::FRAGMENT);
	frag.readShader("src/shader/plane.frag");
	frag.compileShader();

	program.setShaders({ vert.getId(), frag.getId() });
	program.link();

	lookat= glm::lookAt(moving, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::vec4 worldLight = glm::vec4(1.0, 1.0, 1.0, 1.0);
	program.setUniform("LightPosition", lookat * worldLight);

	glViewport(0, 0, width, height);
	//perspective(látószög,képarány,legözelebbi pont amit látunk,legtávolabbi pont amit látunk
	projection = glm::perspective(glm::radians(50.0f), (float)width / height, 0.3f, 100.0f);
	//a modellen végzett transzformációk
	model = glm::mat4(1.0f);


}

void mainloop()
{
	while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glm::mat4 mv = lookat * model;
		program.setUniform("ModelViewMatrix", mv);

		glm::mat3 normalMatrix = glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2]));
		program.setUniform("NormalMatrix", normalMatrix);

		glm::mat4 mvp = projection * lookat * model;
		program.setUniform("MVP", mvp);

		glm::vec3 ld = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 kd = glm::vec3(0.9f, 0.5f, 0.3f);
		program.setUniform("Ld", ld);
		program.setUniform("Kd", kd);



		program.use();
		cube->render();
		//sphere->render();
		//teapot->render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void keyFunction(GLFWwindow *window, int key, int scanCode, int action, int mods)
{
	if (action == GLFW_REPEAT || action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_RIGHT:
			moving.x += 1;
			cout << "x: " << moving.x<<endl;
			break;
		case GLFW_KEY_LEFT:
			moving.x -= 1;
			cout << "x: " << moving.x << endl;
			break;
		case GLFW_KEY_UP:
			moving.y += 1;
			cout << "y: " << moving.y << endl;
			break;
		case GLFW_KEY_DOWN:
			moving.y -= 1;
			cout << "y: " << moving.y << endl;
			break;
		case GLFW_KEY_RIGHT_ALT:
			moving.z += 1;
			cout << "z: " << moving.z << endl;
			break;
		case GLFW_KEY_LEFT_ALT:
			moving.z -= 1;
			cout << "z: " << moving.z << endl;
			break;
		default:
			printf("Bad key :(\n");
			break;
		}
		lookat = glm::lookAt(moving, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		glfwPollEvents();
	}
}

int main()
{
	window = initWindow("Pityunal", width, height);

	init();

	glfwSetKeyCallback(window, keyFunction);

	mainloop();

	return 0;
}