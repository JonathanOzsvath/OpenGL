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
VBOCube *cube;
VBOPlane *plane;

/*vetítési változók*/
mat4 projProj;
mat4 projView;
vec3 projPos;
vec3 projAt;
mat4 projScaleTrans;
int vetit;

//mozgatható model változó
vec3 smallCubePosition;

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
	prog.use();
}

void setMatrices()
{
	mat4 mv = view * model;
	prog.setUniform("Model", model);
	prog.setUniform("ModelView", mv);
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

void drawScene()
{
	prog.setUniform("Light.Position", vec4(0.0f,15.0f,0.0f,1.0f) * camera.getView());

	//projektor homogén koordinátái
	vec4 lightP = vec4(projPos, 1.0f);

	//kamera nézetben a projektor koordinátái
	vec4 lightCamPos = camera.getView() * lightP;

	//kamera nézetben a projektor vetítési helye(ahova vetít)
	vec4 projAtCam = camera.getView() * vec4(projAt - projPos, 1.0);
	vec4 cameraOrigo = camera.getView() * vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vec4 cameraConeVec = glm::normalize(projAtCam - cameraOrigo);

	vec4 lightUp = vec4(0.0, 1.0, 0.0, 1.0);
	vec4 upVecCam = camera.getView() * lightUp;
	vec4 finalUp = glm::normalize(upVecCam - cameraOrigo);

	prog.setUniform("Spot.Position", lightCamPos);
	prog.setUniform("Spot.Direction", vec3(cameraConeVec));
	prog.setUniform("Spot.Up", vec3(finalUp));

	prog.setUniform("Material.Kd", 0.9f, 0.5f, 0.3f);
	prog.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
	prog.setUniform("Material.Ka", 0.9f * 0.3f, 0.5f * 0.3f, 0.3f * 0.3f);
	prog.setUniform("Material.Shininess", 100.0f);

	model = mat4(1.0f);
	model *= glm::translate(smallCubePosition);
	setMatrices();
	cube->render();

	prog.setUniform("Material.Kd", 0.7f, 0.7f, 0.7f);
	prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
	prog.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
	prog.setUniform("Material.Shininess", 100.0f);

	model = mat4(1.0f);
	model *= translate(vec3(0.0f, 0.0f, -15.0f));
	model *= rotate(radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	setMatrices();
	plane->render();

}

void init()
{
	compileShader();
	glClearColor(0.0f, 1.0f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	cube = new VBOCube();
	plane = new VBOPlane(30.0f, 30.0f, 1, 1);

	smallCubePosition = vec3(0.0f, 0.0f, 0.0f);

	prog.setUniform("Spot.Intensity", vec3(0.3f, 0.3f, 0.3f));
	prog.setUniform("Spot.exponent", 5.0f);
	prog.setUniform("Spot.alfa", 18.0f);
	prog.setUniform("Spot.beta", 14.0f);

	prog.setUniform("Light.Intensity", vec3(0.1f, 0.1f, 0.1f));

	/*vetítés init*/
	projPos = vec3(0.0f, 0.0f, 0.0f);
	projAt = vec3(0.0f, 0.0f, -15.0f);
	vec3 projUp = vec3(0.0f, 1.0f, 0.0f);
	projView = glm::lookAt(projPos, projAt, projUp);
	projProj = glm::perspective(30.0f, 1.0f, 0.2f, 1000.0f);

	projScaleTrans = glm::translate(vec3(0.5f)) * glm::scale(vec3(2.0f));
	mat4 m = projScaleTrans * projProj * projView;
	// Set the uniform variable
	prog.setUniform("ProjectorMatrix", m);

	// Load texture file
	glActiveTexture(GL_TEXTURE1);
	//valtas = 0;
	string kepek = "src/texture/mese/Vuk_1.tga";
	const char* tmp = kepek.c_str();
	vetit = TGAIO::loadTex(tmp);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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

		view = camera.getView();
		drawScene();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void keyFunction(GLFWwindow *window, int key, int scanCode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	camera.setKey(key, action, deltaTime);

	GLfloat cubeSpeed = 0.2f;

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
	window = initWindow("negyzetes Spotlight", width, height);

	init();
	resize(width, height);

	glfwSetKeyCallback(window, keyFunction);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	mainloop();

	return 0;
}