#include "GLShader.h"
#include "GLBuffer.h"
#include "GLProgram.h"
#include "GLFWProgram.h"
#include "Texture.h"
#include "Camera.h"

#include "vbosphere.h"
#include "vbomesh.h"
#include "vboplane.h"
#include "tgaio.h"
#include "frustum.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtx\rotate_vector.hpp>

using namespace glm;
using namespace std;

/*ablakoló változók*/
GLFWwindow *window;
GLProgram prog;
GLuint width = 1920, height = 1080;

vec4 worldLight = vec4(0.0f, 0.0f, 0.0f, 1.0f);

/*alap változók*/
mat4 view;
mat4 model;
mat4 projection;

/*kamera változók*/
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
Camera camera(width, height, 25.0f);

/*modell változók*/
VBOPlane *padlo;

//Texture
GLuint padloTexture;

GLuint VAO;

void compileShader()
{
	GLShader vert(GLShader::GLShaderType::VERTEX);
	vert.readShader("src/shader/geometry.vs");
	vert.compileShader();

	GLShader frag(GLShader::GLShaderType::FRAGMENT);
	frag.readShader("src/shader/geometry.fs");
	frag.compileShader();

	GLShader geom(GLShader::GLShaderType::GEOMETRY);
	geom.readShader("src/shader/geometry.gs");
	geom.compileShader();

	prog.setShaders({ vert.getId(), geom.getId(), frag.getId() });
	prog.link();
	prog.use();
}

void setMatrices()
{
	mat4 mv = camera.getView() * model;
	/*prog.setUniform("ModelViewMatrix", mv);
	prog.setUniform("ModelMatrix", model);
	prog.setUniform("NormalMatrix",
		mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));*/
	prog.setUniform("MVP", projection * mv);
}

void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.1f, 300.0f);
}

void init()
{
	compileShader();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	padlo = new VBOPlane(40.0f, 30.0f, 1, 1);

	//padló texture
	/*glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &padloTexture);
	padloTexture = TGAIO::loadTex("src/texture/floor.tga");
	glBindTexture(GL_TEXTURE_2D, padloTexture);*/

	//=============
	GLfloat points[] = {
		-0.5f, 0.5f, 1.0f, 0.0f, 0.0f, // Top-left
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, // Top-right
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // Bottom-right
		-0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // Bottom-left
	};
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glBindVertexArray(0);
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

		/*glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, padloTexture);
		model = mat4(1.0f);
		model *= translate(vec3(0.0f, -10.0f, 0.0f));
		setMatrices();
		padlo->render();*/

		setMatrices();

		glBindVertexArray(VAO);
		glDrawArrays(GL_POINTS, 0, 4);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void keyFunction(GLFWwindow *window, int key, int scanCode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	camera.setKey(key, action, deltaTime);

	GLfloat cubeSpeed = 0.1f;

	if (action == GLFW_REPEAT || action == GLFW_PRESS)
	{
		switch (key)
		{
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
	window = initWindow("Proba", width, height);

	init();
	resize(width, height);

	glfwSetKeyCallback(window, keyFunction);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	mainloop();

	glfwTerminate();
	return 0;
}