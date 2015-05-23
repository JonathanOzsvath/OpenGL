#include "GLShader.h"
#include "GLBuffer.h"
#include "GLProgram.h"
#include "GLFWProgram.h"
#include "vbosphere.h"
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

using namespace glm;
using namespace std;

GLFWwindow *window;
GLProgram prog;
GLuint width = 1920, height = 1080;

vec4 worldLight = vec4(0.0f, 0.0f, 0.0f, 1.0f);

mat4 model;
mat4 projection;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
Camera camera(width, height, 25.0f);

VBOCube *cube;
VBOPlane *plane;

int vetit;

void compileShader()
{
	GLShader vert(GLShader::GLShaderType::VERTEX);
	vert.readShader("src/shader/spot.vs");
	vert.compileShader();

	GLShader frag(GLShader::GLShaderType::FRAGMENT);
	frag.readShader("src/shader/spot.fs");
	frag.compileShader();

	prog.setShaders({ vert.getId(), frag.getId() });
	prog.link();

}

void setMatrices()
{
	mat4 mv = camera.getView() * model;
	prog.setUniform("ModelViewMatrix", mv);
	prog.setUniform("ModelMatrix", model);
	prog.setUniform("NormalMatrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	prog.setUniform("MVP", projection * mv);
}

void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.01f, 300.0f);
}

void init()
{
	compileShader();
	glClearColor(0.0f, 1.0f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	prog.use();
	cube = new VBOCube();
	plane = new VBOPlane(30.0f, 30.0f, 1, 1);

	prog.setUniform("Spot.intensity", vec3(1.0f, 1.0f, 1.0f));
	prog.setUniform("Spot.exponent", 30.0f);
	prog.setUniform("Spot.alfa", 5.0f);
	prog.setUniform("Spot.beta", 5.0f);

	prog.setUniform("LightIntensity", vec3(0.1f, 0.1f, 0.1f));

	/*vetítés init*/
	vec3 projPos = vec3(0.0f, 10.0f, 0.0f);
	vec3 projAt = vec3(0.0f, 0.0f, 0.0f);
	vec3 projUp = vec3(1.0f, 0.0f, 0.0f);
	mat4 projView = glm::lookAt(projPos, projAt, projUp);
	mat4 projProj = glm::perspective(glm::radians(30.0f), 1.0f, 0.2f, 1000.0f);
	mat4 projScaleTrans = glm::translate(vec3(0.5f)) * glm::scale(vec3(0.5f));
	prog.setUniform("ProjectorMatrix", projScaleTrans * projProj * projView);

	// Load texture file
	glActiveTexture(GL_TEXTURE0);
	//valtas = 0;
	string kepek = "src/texture/mese/Vuk_1.tga";
	const char* tmp = kepek.c_str();
	vetit = TGAIO::loadTex(tmp);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
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

		prog.use();

		prog.setUniform("LightPosition", vec4(10.0f)* camera.getView());

		float area = 50.0f;
		vec4 lightPos = vec4(0.0f,5.0f,0.0f, 1.0f);
		prog.setUniform("Spot.position", camera.getView() * lightPos);
		mat3 normalMatrix = mat3(vec3(camera.getView()[0]), vec3(camera.getView()[1]), vec3(camera.getView()[2]));
		prog.setUniform("Spot.direction", normalMatrix * vec3(-lightPos));
		prog.setUniform("Spot.Up", vec3(1.0f, 0.0f, 0.0f));

		prog.setUniform("Kd", 0.9f, 0.5f, 0.3f);
		prog.setUniform("Ks", 0.95f, 0.95f, 0.95f);
		prog.setUniform("Ka", 0.9f * 0.3f, 0.5f * 0.3f, 0.3f * 0.3f);
		prog.setUniform("Shininess", 100.0f);

		model = mat4(1.0f);
		model *= glm::translate(vec3(0.0f, 1.0f, 0.0f));
		model *= glm::rotate(glm::radians(45.0f), vec3(0.0f, 1.0f, 0.0f));
		model *= glm::rotate(glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
		setMatrices();
		cube->render();

		prog.setUniform("Kd", 0.7f, 0.7f, 0.7f);
		prog.setUniform("Ks", 0.9f, 0.9f, 0.9f);
		prog.setUniform("Ka", 0.2f, 0.2f, 0.2f);
		prog.setUniform("Shininess", 100.0f);

		model = mat4(1.0f);
		setMatrices();
		plane->render();

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
	window = initWindow("negyzetes Spotlight", width, height);

	init();
	resize(width, height);

	glfwSetKeyCallback(window, keyFunction);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	mainloop();

	return 0;
}