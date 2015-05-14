#include "GLShader.h"
#include "GLBuffer.h"
#include "GLProgram.h"
#include "GLFWProgram.h"
#include "vboplane.h"
#include "vboteapot.h"
#include "vbomesh.h"
#include "Cube.h"
#include "Texture.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm\gtx\transform.hpp>
#include <glm\gtx\rotate_vector.hpp>
#include "tgaio.h"
#include <vector>

using namespace glm;
using namespace std;

GLFWwindow *window;
GLProgram sphereProg, projTexProg;
GLuint width = 1280, height = 720;

vec4 worldLight = vec4(0.0f, 20.0f, 0.0f, 1.0f);

mat4 model;
mat4 projection;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
Camera camera(width, height, 100.0f);

VBOPlane *padlo, *plafon, *fal1, *fal2, *vaszon;
VBOMesh *asztal, *computer, *sofa, *cupboard, *szek, *darts;
Texture *padloTexture, *fal;

VBOTeapot *teapot;

int vetit, valtas;
vec3 smallCubePosition;

vector<string> kepek;

void compileShader()
{
	GLShader sphereVert(GLShader::GLShaderType::VERTEX);
	sphereVert.readShader("src/shader/phong.vert");
	sphereVert.compileShader();

	GLShader sphereFrag(GLShader::GLShaderType::FRAGMENT);
	sphereFrag.readShader("src/shader/phong.frag");
	sphereFrag.compileShader();

	sphereProg.setShaders({ sphereVert.getId(), sphereFrag.getId() });
	sphereProg.link();

	GLShader projtexVert(GLShader::GLShaderType::VERTEX);
	projtexVert.readShader("src/shader/projtex.vs");
	projtexVert.compileShader();

	GLShader projtexFrag(GLShader::GLShaderType::FRAGMENT);
	projtexFrag.readShader("src/shader/projtex.fs");
	projtexFrag.compileShader();

	projTexProg.setShaders({ projtexVert.getId(), projtexFrag.getId() });
	projTexProg.link();
}

void phongSetMatrices()
{
	glm::mat4 mv = camera.getView() * model;
	sphereProg.setUniform("ModelViewMatrix", mv);
	sphereProg.setUniform("NormalMatrix", mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
	sphereProg.setUniform("MVP", projection * camera.getView() * model);
}

void projtexSetMarices()
{
	mat4 mv = camera.getView() * model;
	projTexProg.setUniform("ModelMatrix", model);
	projTexProg.setUniform("ModelViewMatrix", mv);
	projTexProg.setUniform("NormalMatrix", mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	projTexProg.setUniform("MVP", projection * mv);
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
	glClearColor(0.0f, 1.0f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	for (int i = 1; i <= 30; i++)
	{
		string tmp = "src/texture/mese/Vuk_" + to_string(i) + ".tga";
		kepek.push_back(tmp);
	}

	sphereProg.use();
	padlo = new VBOPlane(50.0f, 70.0f, 10, 10);
	fal1 = new VBOPlane(50.0f, 20.0f, 10, 10);
	fal2 = new VBOPlane(70.0f, 20.0f, 10, 10);
	vaszon = new VBOPlane(10.0f, 10.0f, 10, 10);
	asztal = new VBOMesh("src/desk.obj");
	computer = new VBOMesh("src/computer.obj");
	sofa = new VBOMesh("src/sofa.obj");
	cupboard = new VBOMesh("src/cupboard.obj");
	szek = new VBOMesh("src/szek.obj");
	darts = new VBOMesh("src/darts.obj");
	teapot = new VBOTeapot(5, mat4(1.0f));

	/*phong init*/
	sphereProg.setUniform("Light.Intensity", vec3(0.5f, 0.5f, 0.5f));
	sphereProg.setUniform("Light.Position", camera.getView() * worldLight);
	padloTexture = new Texture("src/texture/floor.tga",0);
	fal = new Texture("src/texture/fal.tga", 0);

	/*vetítés init*/
	projTexProg.use();
	vec3 projPos = vec3(-10.0f, 10.0f, -30.0f);
	vec3 projAt = vec3(0.0f, 10.0f, -30.0f);
	vec3 projUp = vec3(0.0f, 1.0f, 0.0f);
	mat4 projView = glm::lookAt(projPos, projAt, projUp);
	mat4 projProj = glm::perspective(glm::radians(30.0f), 1.0f, 0.2f, 1000.0f);
	mat4 projScaleTrans = glm::translate(vec3(0.5f)) * glm::scale(vec3(0.5f));
	projTexProg.setUniform("ProjectorMatrix", projScaleTrans * projProj * projView);

	// Load texture file
	glActiveTexture(GL_TEXTURE0);
	valtas = 0;
	const char* tmp = kepek[valtas].c_str();
	vetit = TGAIO::loadTex(tmp);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	projTexProg.setUniform("Light.Position", vec4(0.0f, 0.0f, 0.0f, 1.0f));
	projTexProg.setUniform("Light.Intensity", vec3(0.1f, 0.1f, 0.1f));

	smallCubePosition = vec3(0.0f, -1.0f, 0.0f);
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

		sphereProg.use();
		model = mat4(1.0f);
		phongSetMatrices();

		sphereProg.setUniform("Light.Position", camera.getView() * worldLight);
		sphereProg.setUniform("Material.Kd", vec3(0.9f, 0.5f, 0.3f));
		sphereProg.setUniform("Material.Ka", vec3(0.9f, 0.5f, 0.3f));
		sphereProg.setUniform("Material.Ks", vec3(0.8f, 0.8f, 0.8f));
		sphereProg.setUniform("Material.Shininess", 20.0f);
		sphereProg.setUniform("WithTexture", true);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, padloTexture->id);
		padlo->render();

		model = mat4(1.0f);
		model *= rotate(radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
		model *= translate(vec3(0.0f, 35.0f, -10.0f));
		phongSetMatrices();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, fal->id);
		fal1->render();

		model = mat4(1.0f);
		model *= rotate(radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
		model *= translate(vec3(0.0f, -35.0f, -10.0f));
		phongSetMatrices();
		fal1->render();

		model = mat4(1.0f);
		model *= rotate(radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
		model *= rotate(radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
		model *= translate(vec3(0.0f, 25.0f, -10.0f));
		phongSetMatrices();
		fal2->render();

		model = mat4(1.0f);
		model *= rotate(radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
		model *= rotate(radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
		model *= translate(vec3(0.0f, -25.0f, -10.0f));
		phongSetMatrices();
		fal2->render();

		/*plafon*/
		model = mat4(1.0f);
		model *= translate(vec3(0.0f, 20.0f, 0.0f));
		phongSetMatrices();
		padlo->render();

		/*asztal*/
		model = mat4(1.0f);
		model *= scale(vec3(2.5f));
		model *= translate(vec3(-9.0f, 0.0f, -8.5f));
		phongSetMatrices();
		sphereProg.setUniform("Material.Kd", vec3(1.0f, 1.0f, 1.0f));
		sphereProg.setUniform("Material.Ka", vec3(1.0f, 1.0f, 1.0f));
		sphereProg.setUniform("Material.Ks", vec3(1.0f, 1.0f, 1.0f));
		sphereProg.setUniform("WithTexture", false);
		asztal->render();

		/*számítógép*/
		model = mat4(1.0f);
		model *= translate(vec3(-14.0f, 4.6f, -34.0f));
		phongSetMatrices();
		sphereProg.setUniform("Material.Kd", vec3(0.7f, 0.5f, 0.0f));
		sphereProg.setUniform("Material.Ka", vec3(0.3f, 0.5f, 0.8f));
		sphereProg.setUniform("Material.Ks", vec3(1.0f, 1.0f, 1.0f));
		computer->render();
		
		/*kanapék*/
		model = mat4(1.0f);
		model *= scale(vec3(3.0f));
		model *= rotate(radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
		model *= translate(vec3(-5.0f, 0.0f, -7.3f));
		phongSetMatrices();
		sphereProg.setUniform("Material.Kd", vec3(0.56f, 0.32f, 0.25f));
		sphereProg.setUniform("Material.Ka", vec3(0.56f, 0.32f, 0.25f));
		sphereProg.setUniform("Material.Ks", vec3(0.7f, 0.5f, 0.03f));
		sofa->render();
		model = mat4(1.0f);
		model *= scale(vec3(3.0f));
		model *= rotate(radians(-90.0f), vec3(0.0f, 1.0f, 0.0f));
		model *= translate(vec3(0.0f, 0.0f, -7.3f));
		phongSetMatrices();
		sofa->render();

		/*szekrények*/
		model = mat4(1.0f);
		model *= scale(vec3(3.0f));
		model *= rotate(radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
		model *= translate(vec3(-7.5f, 0.0f, -5.3f));
		phongSetMatrices();
		sphereProg.setUniform("Material.Kd", vec3(0.56f, 0.32f, 0.25f));
		sphereProg.setUniform("Material.Ka", vec3(0.56f, 0.32f, 0.25f));
		sphereProg.setUniform("Material.Ks", vec3(0.7f, 0.5f, 0.03f));
		cupboard->render();
		model = mat4(1.0f);
		model *= scale(vec3(3.0f));
		model *= rotate(radians(180.0f), vec3(0.0f, 1.0f, 0.0f));
		model *= translate(vec3(-7.5f, 0.0f, -9.6f));
		phongSetMatrices();
		cupboard->render();

		/*szék*/
		model = mat4(1.0f);
		model *= scale(vec3(0.8f));
		model *= translate(vec3(-13.0f, 0.0f, -28.0f));
		model *= rotate(radians(25.0f), vec3(0.0f, 1.0f, 0.0f));

		phongSetMatrices();
		sphereProg.setUniform("Material.Kd", vec3(0.3f, 0.3f, 0.9f));
		sphereProg.setUniform("Material.Ka", vec3(0.56f, 0.32f, 0.25f));
		sphereProg.setUniform("Material.Ks", vec3(0.7f, 0.5f, 0.03f));
		szek->render();

		/*darts*/
		model = mat4(1.0f);
		model *= translate(vec3(0.0f, 10.5f, 37.0f));
		model *= rotate(radians(92.0f), vec3(0.0f, 0.0f, 1.0f));
		model *= rotate(radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
		phongSetMatrices();
		sphereProg.setUniform("Material.Kd", vec3(1.0f, 1.0f, 1.0f));
		sphereProg.setUniform("Material.Ka", vec3(1.0f, 1.0f, 1.0f));
		sphereProg.setUniform("Material.Ks", vec3(1.0f, 1.0f, 1.0f));
		darts->render();

		/*vászon*/
		projTexProg.use();
		projTexProg.setUniform("Material.Kd", 1.0f, 0.2f, 0.1f);
		projTexProg.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
		projTexProg.setUniform("Material.Ka", 0.1f, 0.1f, 0.1f);
		projTexProg.setUniform("Material.Shininess", 50.0f);

		model = mat4(1.0f);
		model *= translate(vec3(0.0f, 10.0f, -30.0f));
		model *= rotate(radians(90.0f), vec3(0.0f, 0.0f, 1.0f));

		projtexSetMarices();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, vetit);
		vaszon->render();

		model = mat4(1.0f);
		model *= translate(smallCubePosition);
		model *= glm::rotate(glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
		projtexSetMarices();
		teapot->render();

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
			smallCubePosition += vec3(0.0f, cubeSpeed, 0.0f);
			break;
		case GLFW_KEY_DOWN:
			smallCubePosition -= vec3(0.0f, cubeSpeed, 0.0f);
			break;
		case GLFW_KEY_RIGHT:
			smallCubePosition += vec3(cubeSpeed, 0.0f, 0.0f);
			break;
		case GLFW_KEY_LEFT:
			smallCubePosition -= vec3(cubeSpeed, 0.0f, 0.0f);
			break;
		case GLFW_KEY_M:
			smallCubePosition += vec3(0.0f, 0.0f, cubeSpeed);
			break;
		case GLFW_KEY_N:
			smallCubePosition -= vec3(0.0f, 0.0f, cubeSpeed);
			break;
		case GLFW_KEY_O:
			if (valtas < 29){
				valtas++;
				glActiveTexture(GL_TEXTURE0);
				const char* tmp = kepek[valtas].c_str();
				vetit = TGAIO::loadTex(tmp);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			}
			break;
		case GLFW_KEY_I:
			if (valtas > 0){
				valtas--;
				glActiveTexture(GL_TEXTURE0);
				const char* tmp = kepek[valtas].c_str();
				vetit = TGAIO::loadTex(tmp);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			}
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
	window = initWindow("Room", width, height);

	init();
	resize(width, height);

	glfwSetKeyCallback(window, keyFunction);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	mainloop();

	return 0;
}