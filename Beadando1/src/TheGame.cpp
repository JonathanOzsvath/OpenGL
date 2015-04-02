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

vec4 worldLight = vec4(0.0f, 0.0f, 0.0f, 1.0f);

mat4 model;
mat4 projection;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
Camera camera(width, height, 25.0f);
Cube *cube;
VBOSphere *bigSphere, *smallSphere;

Texture *texture, *texture0, *texture1, *texture2, *texture3, *texture4, *texture5, *texture6, *texture7, *texture8, *texture9;

vector<vec3> modelStack;
vector<int> textureID,textureID2;
GLfloat mert1[10];
GLfloat mert2[10];
GLfloat mert3[10];

void compileShader()
{
	GLShader envVert(GLShader::GLShaderType::VERTEX);
	envVert.readShader("src/shader/cubemap.vert");
	envVert.compileShader();

	GLShader envFrag(GLShader::GLShaderType::FRAGMENT);
	envFrag.readShader("src/shader/cubemap.frag");
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
	projection = glm::perspective(glm::radians(60.0f), (float)w / h, 3.0f, 300.0f);
}

void init()
{
	compileShader();
	glClearColor(0.0f, 1.0f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	cube = new Cube(50.0f, false);
	envProg.use();
	envProg.setUniform("LightPosition", vec4(0.0f, 0.0f, 0.0f, 1.0f));
	envProg.setUniform("Kd", vec3(0.9f, 0.5f, 0.3f));
	envProg.setUniform("Ld", vec3(1.0f, 1.0f, 1.0f));
	//CubeMapTexture cubetexture("src/texture/brick/brick", 1024);
	CubeMapTexture cubetexture("src/texture/fu/fu", 768);

	bigSphere = new VBOSphere(3.0f, 360, 360);
	smallSphere = new VBOSphere(1.0f, 360, 360);
	sphereProg.use();
	sphereProg.setUniform("Light.Intensity", vec3(0.5f, 0.5f, 0.5f));
	sphereProg.setUniform("Light.Position", camera.getView() * worldLight);

	for (size_t i = 0; i < 10; i++)
	{
		mert1[i] = (rand() % 48 - 24);
		mert2[i] = (rand() % 48 - 24);
		mert3[i] = (rand() % 48 - 24);
		modelStack.push_back(vec3(mert1[i], mert2[i], mert3[i]));
	}

	texture = new Texture("src/texture/basketball.tga", 0);
	texture0 = new Texture("src/texture/brazuka.tga", 0);
	textureID.push_back(texture0->id);
	texture1 = new Texture("src/texture/eltrusko.tga", 1);
	textureID.push_back(texture1->id);
	texture2 = new Texture("src/texture/europass.tga", 2);
	textureID.push_back(texture2->id);
	texture3 = new Texture("src/texture/fevernova.tga", 3);
	textureID.push_back(texture3->id);
	texture4 = new Texture("src/texture/jabulani.tga", 4);
	textureID.push_back(texture4->id);
	texture5 = new Texture("src/texture/tango12.tga", 5);
	textureID.push_back(texture5->id);
	texture6 = new Texture("src/texture/Teamgeist.tga", 6);
	textureID.push_back(texture6->id);
	texture7 = new Texture("src/texture/telstar.tga", 7);
	textureID.push_back(texture7->id);
	texture8 = new Texture("src/texture/roteiro.tga", 8);
	textureID.push_back(texture8->id);
	texture9 = new Texture("src/texture/premierLeague.tga", 9);
	textureID.push_back(texture9->id);
	textureID2 = textureID;
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
		envProg.setUniform("WorldCameraPosition", vec3(0.0f, 0.0f, 0.0f));
		envProg.setUniform("DrawSkyBox", true);
		cube->render();

		model = mat4(1.0f);
		model *= translate(camera.getSpherePosition());
		sphereProg.use();
		sphereSetMatrices();

		sphereProg.setUniform("Light.Position", camera.getView() * worldLight);
		sphereProg.setUniform("Material.Kd", vec3(0.2f, 0.2f, 0.2f));
		sphereProg.setUniform("Material.Ka", vec3(0.5f, 0.5f, 0.5f));
		sphereProg.setUniform("Material.Ks", vec3(1.0f, 1.0f, 1.0f));
		sphereProg.setUniform("Material.Shininess", 50.0f);

		glBindTexture(GL_TEXTURE_2D, texture->id);
		bigSphere->render();


		for (int i = 0; i < modelStack.size(); i++)
		{
			model = mat4(1.0f);
			model *= translate(modelStack[i]);
			sphereSetMatrices();
			glBindTexture(GL_TEXTURE_2D, textureID[i]);
			smallSphere->render();
		}

		
		for (int i = 0; i < modelStack.size(); i++)
		{
			if (glm::distance(modelStack[i], camera.getSpherePosition()) < 4.0f)
			{
				modelStack.erase(modelStack.begin() + i);
				textureID.erase(textureID.begin() + i);
			}
		}

		if (modelStack.empty())
		{
			cout << "TheGame end! :)" << endl;
			exit(0);
		}

		/*model = mat4(1.0f);
		model *= translate(vec3(10.0f, 10.0f, 0.0f));
		sphereSetMatrices();
		glBindTexture(GL_TEXTURE_2D, texture2->id);
		smallSphere->render();

		model = mat4(1.0f);
		model *= translate(vec3(10.0f, 10.0f, 10.0f));
		sphereSetMatrices();
		glBindTexture(GL_TEXTURE_2D, texture3->id);
		smallSphere->render();*/

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void keyFunction(GLFWwindow *window, int key, int scanCode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	camera.setKey(key, action, deltaTime);
	switch (key)
	{
	case GLFW_KEY_R:
		modelStack.clear();
		textureID.clear();
		for (size_t i = 0; i < 10; i++)
		{
			mert1[i] = (rand() % 48 - 24);
			mert2[i] = (rand() % 48 - 24);
			mert3[i] = (rand() % 48 - 24);
			modelStack.push_back(vec3(mert1[i], mert2[i], mert3[i]));

			textureID = textureID2;
		}
		break;
	default:
		break;
	}

	glfwPollEvents();
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	camera.setMouse(xpos, ypos);
}

int main()
{
	window = initWindow("TheGame", width, height);

	init();
	resize(width, height);

	glfwSetKeyCallback(window, keyFunction);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	mainloop();

	return 0;
}