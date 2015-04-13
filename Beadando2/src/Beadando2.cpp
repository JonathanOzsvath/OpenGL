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
#include <glm\gtx\rotate_vector.hpp>
#include "tgaio.h"

using namespace glm;
using namespace std;

GLFWwindow *window;
GLProgram envProg;
GLProgram sphereProg;
GLuint width = 1920, height = 1080;

vec4 worldLight = vec4(0.0f, 0.0f, 5.0f, 1.0f);

mat4 model;
mat4 projection;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
Camera camera(width, height, 25.0f);
Cube *cube, *smallCube;
GLfloat area = 50.0f, sArea = 10.0f;

VBOSphere *sphere;
Texture *ball, *smallCubeTexture;
GLfloat radius = 1.0f;

vec3 ballPosition, smallCubePosition;
vec3 direction;

void compileShader()
{
	GLShader envVert(GLShader::GLShaderType::VERTEX);
	envVert.readShader("src/shader/texture.vert");
	envVert.compileShader();

	GLShader envFrag(GLShader::GLShaderType::FRAGMENT);
	envFrag.readShader("src/shader/texture.frag");
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
	projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 300.0f);
}

void init()
{
	compileShader();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	cube = new Cube(area, false);
	envProg.use();
	envProg.setUniform("LightPosition", vec4(0.0f, 0.0f, 0.0f, 1.0f));
	envProg.setUniform("Kd", vec3(0.9f, 0.5f, 0.3f));
	envProg.setUniform("Ld", vec3(1.0f, 1.0f, 1.0f));

	smallCube = new Cube(sArea, true);
	sphere = new VBOSphere(radius, 360, 360);
	sphereProg.use();
	sphereProg.setUniform("Light.Intensity", vec3(0.5f, 0.5f, 0.5f));
	sphereProg.setUniform("Light.Position", camera.getView() * worldLight);

	CubeMapTexture cubetexture("src/texture/brick/brick", 1024);
	ball = new Texture("src/texture/BeachBallTexture.tga", 0);
	smallCubeTexture = new Texture("src/texture/flower.tga", 0);

	smallCubePosition = vec3(0.0f, -10.0f, 0.0f);
	ballPosition = vec3(0.0f);
	direction = vec3(0.01f, 0.03f, 0.1f);
}

GLfloat dist(vec3 a, vec3 b)
{
	return (b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y) + (b.y - a.z)*(b.y - a.z);
}

GLfloat distPointPlane(vec3 point, vec3 cornelPosition, vec3 normalVector)
{
	GLfloat A = normalVector.x;
	GLfloat B = normalVector.y;
	GLfloat C = normalVector.z;
	GLfloat D = normalVector.x*cornelPosition.x + normalVector.y*cornelPosition.y + normalVector.z*cornelPosition.z;
	float nevezo = sqrt(A*A+B*B+C*C);
	float dist = (A*point.x + B*point.y + C*point.z - D) / nevezo;
	return dist;
}

void moveBall()
{

	if (ballPosition.x > (area/2.0f) - radius)
	{
		direction = reflect(direction, vec3(-1.0f, 0.0f, 0.0f));
	}
	if (ballPosition.x < -(area / 2.0f) + radius)
	{
		direction = reflect(direction, vec3(-1.0f, 0.0f, 0.0f));
	}
	if (ballPosition.y > (area / 2.0f) - radius)
	{
		direction = reflect(direction, vec3(0.0f, -1.0f, 0.0f));
	}
	if (ballPosition.y < -(area / 2.0f) + radius)
	{
		direction = reflect(direction, vec3(0.0f, -1.0f, 0.0f));
	}
	if (ballPosition.z > (area / 2.0f) - radius)
	{
		direction = reflect(direction, vec3(0.0f, 0.0f, -1.0f));
	}
	if (ballPosition.z < -(area / 2.0f) + radius)
	{
		direction = reflect(direction, vec3(0.0f, 0.0f, -1.0f));
	}

	/*felülrõl*/
	if (abs(distPointPlane(ballPosition, smallCubePosition + vec3(0.0f, (sArea/2.0f), 0.0f), vec3(0.0f, 1.0f, 0.0f))) < radius &&
		distPointPlane(ballPosition, smallCubePosition + vec3(0.0f, (sArea/2.0f), 0.0f), vec3(0.0f, 1.0f, 0.0f))*distPointPlane(smallCubePosition, smallCubePosition + vec3(0.0f, (sArea/2.0f), 0.0f), vec3(0.0f, 1.0f, 0.0f)) < 0 &&
		ballPosition.x <= smallCubePosition.x + (sArea/2.0f) && ballPosition.x >= smallCubePosition.x - (sArea/2.0f) &&
		ballPosition.z <= smallCubePosition.z + (sArea/2.0f) && ballPosition.z >= smallCubePosition.z - (sArea/2.0f))
	{
		direction = reflect(direction, vec3(0.0f, -1.0f, 0.0f));
	}
	/*alulról*/
	if (abs(distPointPlane(ballPosition, smallCubePosition + vec3(0.0f, -(sArea/2.0f), 0.0f), vec3(0.0f, -1.0f, 0.0f))) < radius &&
		distPointPlane(ballPosition, smallCubePosition + vec3(0.0f, -(sArea / 2.0f), 0.0f), vec3(0.0f, -1.0f, 0.0f))*distPointPlane(smallCubePosition, smallCubePosition + vec3(0.0f, -(sArea / 2.0f), 0.0f), vec3(0.0f, -1.0f, 0.0f)) < 0 &&
		ballPosition.x <= smallCubePosition.x + (sArea/2.0f) && ballPosition.x > smallCubePosition.x - (sArea/2.0f) &&
		ballPosition.z <= smallCubePosition.z + (sArea/2.0f) && ballPosition.z > smallCubePosition.z - (sArea/2.0f))
	{
		direction = reflect(direction, vec3(0.0f, -1.0f, 0.0f));
	}
	/*jobbról*/
	if (abs(distPointPlane(ballPosition, smallCubePosition + vec3((sArea / 2.0f), 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f))) < radius &&
		distPointPlane(ballPosition, smallCubePosition + vec3((sArea / 2.0f), 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f))*distPointPlane(smallCubePosition, smallCubePosition + vec3((sArea / 2.0f), 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)) < 0 &&
		ballPosition.y <= smallCubePosition.y + (sArea/2.0f) && ballPosition.y > smallCubePosition.y - (sArea/2.0f) &&
		ballPosition.z <= smallCubePosition.z + (sArea/2.0f) && ballPosition.z > smallCubePosition.z - (sArea/2.0f))
	{
		direction = reflect(direction, vec3(-1.0f, 0.0f, 0.0f));
	}
	/*balról*/
	if (abs(distPointPlane(ballPosition, smallCubePosition + vec3(-(sArea / 2.0f), 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f))) < radius &&
		distPointPlane(ballPosition, smallCubePosition + vec3(-(sArea / 2.0f), 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f))*distPointPlane(smallCubePosition, smallCubePosition + vec3(-(sArea / 2.0f), 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f)) < 0 &&
		ballPosition.y <= smallCubePosition.y + (sArea/2.0f) && ballPosition.y > smallCubePosition.y - (sArea/2.0f) &&
		ballPosition.z <= smallCubePosition.z + (sArea/2.0f) && ballPosition.z > smallCubePosition.z - (sArea/2.0f))
	{
		direction = reflect(direction, vec3(-1.0f, 0.0f, 0.0f));
	}
	/*elõröl*/
	if (abs(distPointPlane(ballPosition, smallCubePosition + vec3(0.0f, 0.0f, (sArea / 2.0f)), vec3(0.0f, 0.0f, 1.0f))) < radius &&
		distPointPlane(ballPosition, smallCubePosition + vec3(0.0f, 0.0f, (sArea / 2.0f)), vec3(0.0f, 0.0f, 1.0f))*distPointPlane(smallCubePosition, smallCubePosition + vec3(0.0f, 0.0f, (sArea / 2.0f)), vec3(0.0f, 0.0f, 1.0f)) < 0 &&
		ballPosition.y <= smallCubePosition.y + (sArea/2.0f) && ballPosition.y > smallCubePosition.y - (sArea/2.0f) &&
		ballPosition.x<= smallCubePosition.x+ (sArea/2.0f) && ballPosition.x> smallCubePosition.x- (sArea/2.0f))
	{
		direction = reflect(direction, vec3(0.0f, 0.0f, -1.0f));
	}
	/*hátulról*/
	if (abs(distPointPlane(ballPosition, smallCubePosition + vec3(0.0f, 0.0f, -(sArea / 2.0f)), vec3(0.0f, 0.0f, -1.0f))) < radius &&
		distPointPlane(ballPosition, smallCubePosition + vec3(0.0f, 0.0f, -(sArea / 2.0f)), vec3(0.0f, 0.0f, -1.0f))*distPointPlane(smallCubePosition, smallCubePosition + vec3(0.0f, 0.0f, -(sArea / 2.0f)), vec3(0.0f, 0.0f, -1.0f)) < 0 &&
		ballPosition.y <= smallCubePosition.y + (sArea/2.0f) && ballPosition.y > smallCubePosition.y - (sArea/2.0f) &&
		ballPosition.x <= smallCubePosition.x + (sArea/2.0f) && ballPosition.x > smallCubePosition.x - (sArea/2.0f))
	{
		direction = reflect(direction, vec3(0.0f, 0.0f, -1.0f));
	}

	ballPosition += direction;
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

		envProg.use();
		envSetMatrices();
		envProg.setUniform("WorldCameraPosition", vec3(0.0f, 0.0f, 0.0f));
		envProg.setUniform("DrawSkyBox", true);
		cube->render();
		
		moveBall();
		model = mat4(1.0f);
		model *= translate(ballPosition);
		sphereProg.use();
		sphereSetMatrices();

		sphereProg.setUniform("Light.Position", camera.getView() * worldLight);
		sphereProg.setUniform("Material.Kd", vec3(0.2f, 0.2f, 0.2f));
		sphereProg.setUniform("Material.Ka", vec3(0.5f, 0.5f, 0.5f));
		sphereProg.setUniform("Material.Ks", vec3(1.0f, 1.0f, 1.0f));
		sphereProg.setUniform("Material.Shininess", 50.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ball->id);
		sphere->render();

		model = mat4(1.0f);
		model *= translate(smallCubePosition);
		sphereSetMatrices();
		glBindTexture(GL_TEXTURE_2D, smallCubeTexture->id);
		smallCube->render();

		//camera.setCameraFront(ballPosition);
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
			if (smallCubePosition.y < (area/2.0f) - sArea/2.0f)
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