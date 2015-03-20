#include <iostream>
#include "GLFWProgram.h"
#include "GLProgram.h"
#include "GLShader.h"
#include "vbocube.h"
#include "vbosphere.h"
#include "vboteapot.h"
#include "vboplane.h"
#include "vbotorus.h"
#include "vbomesh.h"
#include <glm/gtx/transform.hpp>

using namespace std;
using namespace glm;

GLFWwindow *window;
GLuint width = 1280, height = 720;
VBOCube *cube;
VBOPlane *plane;
VBOSphere *sphere;
VBOTeapot *teapot;
VBOTorus *torus;
VBOMesh *mesh;
GLProgram program;
GLProgram program2;

glm::mat4 lookat;
glm::mat4 projection;
glm::mat4 model;
glm::mat4 model2;

glm::vec4 worldLight;
GLfloat phi, theta, r = 3.0;
glm::vec3 moving(r*sinf(radians(phi))*cosf(radians(theta)), r*sinf(radians(phi))*sinf(radians(theta)), r*cosf(radians(phi)));


void init()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	
	glm::mat4 tmp(1.0);
	cube = new VBOCube();
	plane = new VBOPlane(8.0f, 8.0f, 1, 1);
	sphere = new VBOSphere(0.5f, 360, 360);
	teapot = new VBOTeapot(3, tmp);
	torus = new VBOTorus(0.8f, 0.4f, 100, 100);
	//mesh = new VBOMesh("src/untitled.obj");

	GLShader vert(GLShader::GLShaderType::VERTEX);
	vert.readShader("src/shader/diffuse.vert");
	vert.compileShader();

	GLShader frag(GLShader::GLShaderType::FRAGMENT);
	frag.readShader("src/shader/diffuse.frag");
	frag.compileShader();

	program.setShaders({ vert.getId(), frag.getId()});
	program.link();

	GLShader vert2(GLShader::GLShaderType::VERTEX);
	vert2.readShader("src/shader/phong.vert");
	vert2.compileShader();

	GLShader frag2(GLShader::GLShaderType::FRAGMENT);
	frag2.readShader("src/shader/phong.frag");
	frag2.compileShader();

	program2.setShaders({ vert2.getId(), frag2.getId() });
	program2.link();

	lookat= glm::lookAt(moving, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	worldLight = glm::vec4(5.0, 1.0, 10.0, 1.0);
	program.setUniform("LightPosition", lookat * worldLight);
	program2.setUniform("LightPosition", lookat * worldLight);

	glViewport(0, 0, width, height);
	//perspective(látószög,képarány,legözelebbi pont amit látunk,legtávolabbi pont amit látunk
	projection = glm::perspective(glm::radians(50.0f), (float)width / height, 0.3f, 100.0f);
	//a modellen végzett transzformációk

	

	model = glm::mat4(1.0f);
	model *= glm::translate(glm::vec3(0.8f, 0.0f,-1.0f));
	model2 = glm::mat4(1.0f);
	model2 *= glm::translate(glm::vec3(0.0f, 0.0f,-0.8f));
	model2 *= glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
}

void mainloop()
{
	while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		program.use();
		glm::mat4 mv = lookat * model;
		program.setUniform("ModelViewMatrix", mv);

		glm::mat3 normalMatrix = glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2]));
		program.setUniform("NormalMatrix", normalMatrix);

		glm::mat4 mvp = projection * lookat * model;
		program.setUniform("MVP", mvp);

		glm::vec3 ld = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 kd = glm::vec3(1.0f, 1.0f, 0.0f);
		program.setUniform("Ld", ld);
		program.setUniform("Kd", kd);

		//cube->render();
		plane->render();
		//sphere->render();
		torus->render();
		//teapot->render();
		//mesh->render();

		program2.use();

		program2.setUniform("Material.Kd", vec3(0.0f, 1.0f, 0.0f));
		program2.setUniform("Light.Ld", vec3(1.0f, 1.0f, 1.0f));
		program2.setUniform("Material.Ka", lookat * worldLight);
		program2.setUniform("Light.La", vec3(1.0f, 1.0f, 0.0f));
		program2.setUniform("Material.Ks", vec3(0.8f, 0.8f, 0.8f));
		program2.setUniform("Light.Ls", vec3(1.0f, 1.0f, 1.0f));
		program2.setUniform("Material.Shininess", 100.0f);


		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glm::mat4 mv2 = lookat * model2;
		program2.setUniform("ModelViewMatrix", mv2);

		//glm::mat3 normalMatrix2 = glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2]));
		program2.setUniform("NormalMatrix", normalMatrix);

		glm::mat4 mvp2 = projection * lookat * model2;
		program2.setUniform("MVP", mvp2);

		glm::vec3 ld2 = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec3 kd2 = glm::vec3(1.0f, 0.0f, 0.0f);
		program2.setUniform("Ld", ld2);
		program2.setUniform("Kd", kd2);

		
		torus->render();

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
			phi+=0.5;
			moving = vec3(r*sinf(radians(phi))*cosf(radians(theta)), r*sinf(radians(phi))*sinf(radians(theta)), r*cosf(radians(phi)));
			cout << "phi: " << phi << endl;
			break;
		case GLFW_KEY_LEFT:
			phi-=0.5;
			moving = vec3(r*sinf(radians(phi))*cosf(radians(theta)), r*sinf(radians(phi))*sinf(radians(theta)), r*cosf(radians(phi)));
			cout << "phi: " << phi << endl;
			break;
		case GLFW_KEY_UP:
			theta+=0.5;
			moving = vec3(r*sinf(radians(phi))*cosf(radians(theta)), r*sinf(radians(phi))*sinf(radians(theta)), r*cosf(radians(phi)));
			cout << "theta: " << theta << endl;
			break;
		case GLFW_KEY_DOWN:
			theta-=0.5;
			moving = vec3(r*sinf(radians(phi))*cosf(radians(theta)), r*sinf(radians(phi))*sinf(radians(theta)), r*cosf(radians(phi)));
			cout << "theta: " << theta << endl;
			break;
		case GLFW_KEY_D:
			moving.x += 1;
			cout << "x: " << moving.x<<endl;
			break;
		case GLFW_KEY_A:
			moving.x -= 1;
			cout << "x: " << moving.x << endl;
			break;
		case GLFW_KEY_W:
			moving.y += 1;
			cout << "y: " << moving.y << endl;
			break;
		case GLFW_KEY_S:
			moving.y -= 1;
			cout << "y: " << moving.y << endl;
			break;
		case GLFW_KEY_PERIOD:
			moving.z += 1;
			cout << "z: " << moving.z << endl;
			break;
		case GLFW_KEY_COMMA:
			moving.z -= 1;
			cout << "z: " << moving.z << endl;
			break;
		default:
			printf("Bad key :(\n");
			break;
		}

		lookat = glm::lookAt(moving, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

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