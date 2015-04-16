#include "GLShader.h"
#include "GLBuffer.h"
#include "GLProgram.h"
#include "GLFWProgram.h"
#include "vbosphere.h"
#include "Cube.h"
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
Texture *cubeTexture;
GLuint brick;
GLuint cement;
GLuint beachBallTex;

GLfloat radius = 1.0f;

vec3 ballPosition, smallCubePosition;
vec3 direction;
vec3 ballRotation;
float ballAngle;

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
	prog.setUniform("NormalMatrix",
		mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	prog.setUniform("MVP", projection * mv);
}

void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.3f, 300.0f);
}

GLfloat dist(vec3 a, vec3 b)
{
	return (b.x - a.x)*(b.x - a.x) + (b.y - a.y)*(b.y - a.y) + (b.y - a.z)*(b.y - a.z);
}

GLfloat distPointPlane(vec3 point, vec3 cornelPosition, vec3 normalVector)
{
	/*GLfloat A = normalVector.x;
	GLfloat B = normalVector.y;
	GLfloat C = normalVector.z;
	GLfloat D = normalVector.x*cornelPosition.x + normalVector.y*cornelPosition.y + normalVector.z*cornelPosition.z;
	float nevezo = sqrt(A*A + B*B + C*C);
	float dist = (A*point.x + B*point.y + C*point.z - D) / nevezo;
	return dist;*/
	return dot(point - cornelPosition, normalVector);
}

void moveBall()
{

	if (ballPosition.x > (area / 2.0f) - radius)
	{
		direction = reflect(direction, vec3(-1.0f, 0.0f, 0.0f));
		ballRotation = cross(direction, vec3(-1.0f, 0.0f, 0.0f));
	}
	if (ballPosition.x < -(area / 2.0f) + radius)
	{
		direction = reflect(direction, vec3(-1.0f, 0.0f, 0.0f));
		ballRotation = cross(direction, vec3(-1.0f, 0.0f, 0.0f));
	}
	if (ballPosition.y >(area / 2.0f) - radius)
	{
		direction = reflect(direction, vec3(0.0f, -1.0f, 0.0f));
		ballRotation = cross(direction, vec3(0.0f, -1.0f, 0.0f));
	}
	if (ballPosition.y < -(area / 2.0f) + radius)
	{
		direction = reflect(direction, vec3(0.0f, -1.0f, 0.0f));
		ballRotation = cross(direction, vec3(0.0f, -1.0f, 0.0f));
	}
	if (ballPosition.z >(area / 2.0f) - radius)
	{
		direction = reflect(direction, vec3(0.0f, 0.0f, -1.0f));
		ballRotation = cross(direction, vec3(0.0f, 0.0f, -1.0f));
	}
	if (ballPosition.z < -(area / 2.0f) + radius)
	{
		direction = reflect(direction, vec3(0.0f, 0.0f, -1.0f));
		ballRotation = cross(direction, vec3(0.0f, 0.0f, -1.0f));
	}

	/*felülrõl*/
	if (abs(distPointPlane(ballPosition, smallCubePosition + vec3(0.0f, (sArea / 2.0f), 0.0f), vec3(0.0f, 1.0f, 0.0f))) < radius &&
		distPointPlane(ballPosition, smallCubePosition + vec3(0.0f, (sArea / 2.0f), 0.0f), vec3(0.0f, 1.0f, 0.0f))*distPointPlane(smallCubePosition, smallCubePosition + vec3(0.0f, (sArea / 2.0f), 0.0f), vec3(0.0f, 1.0f, 0.0f)) < 0 &&
		ballPosition.x <= smallCubePosition.x + (sArea / 2.0f) && ballPosition.x >= smallCubePosition.x - (sArea / 2.0f) &&
		ballPosition.z <= smallCubePosition.z + (sArea / 2.0f) && ballPosition.z >= smallCubePosition.z - (sArea / 2.0f))
	{
		direction = reflect(direction, vec3(0.0f, 1.0f, 0.0f));
		ballRotation = cross(direction, vec3(0.0f, 1.0f, 0.0f));
	}
	/*alulról*/
	if (abs(distPointPlane(ballPosition, smallCubePosition + vec3(0.0f, -(sArea / 2.0f), 0.0f), vec3(0.0f, -1.0f, 0.0f))) < radius &&
		distPointPlane(ballPosition, smallCubePosition + vec3(0.0f, -(sArea / 2.0f), 0.0f), vec3(0.0f, -1.0f, 0.0f))*distPointPlane(smallCubePosition, smallCubePosition + vec3(0.0f, -(sArea / 2.0f), 0.0f), vec3(0.0f, -1.0f, 0.0f)) < 0 &&
		ballPosition.x <= smallCubePosition.x + (sArea / 2.0f) && ballPosition.x > smallCubePosition.x - (sArea / 2.0f) &&
		ballPosition.z <= smallCubePosition.z + (sArea / 2.0f) && ballPosition.z > smallCubePosition.z - (sArea / 2.0f))
	{
		direction = reflect(direction, vec3(0.0f, 1.0f, 0.0f));
		ballRotation = cross(direction, vec3(0.0f, 1.0f, 0.0f));
	}
	/*jobbról*/
	if (abs(distPointPlane(ballPosition, smallCubePosition + vec3((sArea / 2.0f), 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f))) < radius &&
		distPointPlane(ballPosition, smallCubePosition + vec3((sArea / 2.0f), 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f))*distPointPlane(smallCubePosition, smallCubePosition + vec3((sArea / 2.0f), 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f)) < 0 &&
		ballPosition.y <= smallCubePosition.y + (sArea / 2.0f) && ballPosition.y > smallCubePosition.y - (sArea / 2.0f) &&
		ballPosition.z <= smallCubePosition.z + (sArea / 2.0f) && ballPosition.z > smallCubePosition.z - (sArea / 2.0f))
	{
		direction = reflect(direction, vec3(1.0f, 0.0f, 0.0f));
		ballRotation = cross(direction, vec3(1.0f, 0.0f, 0.0f));
	}
	/*balról*/
	if (abs(distPointPlane(ballPosition, smallCubePosition + vec3(-(sArea / 2.0f), 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f))) < radius &&
		distPointPlane(ballPosition, smallCubePosition + vec3(-(sArea / 2.0f), 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f))*distPointPlane(smallCubePosition, smallCubePosition + vec3(-(sArea / 2.0f), 0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f)) < 0 &&
		ballPosition.y <= smallCubePosition.y + (sArea / 2.0f) && ballPosition.y > smallCubePosition.y - (sArea / 2.0f) &&
		ballPosition.z <= smallCubePosition.z + (sArea / 2.0f) && ballPosition.z > smallCubePosition.z - (sArea / 2.0f))
	{
		direction = reflect(direction, vec3(1.0f, 0.0f, 0.0f));
		ballRotation = cross(direction, vec3(1.0f, 0.0f, 0.0f));
	}
	/*elõröl*/
	if (abs(distPointPlane(ballPosition, smallCubePosition + vec3(0.0f, 0.0f, (sArea / 2.0f)), vec3(0.0f, 0.0f, 1.0f))) < radius &&
		distPointPlane(ballPosition, smallCubePosition + vec3(0.0f, 0.0f, (sArea / 2.0f)), vec3(0.0f, 0.0f, 1.0f))*distPointPlane(smallCubePosition, smallCubePosition + vec3(0.0f, 0.0f, (sArea / 2.0f)), vec3(0.0f, 0.0f, 1.0f)) < 0 &&
		ballPosition.y <= smallCubePosition.y + (sArea / 2.0f) && ballPosition.y > smallCubePosition.y - (sArea / 2.0f) &&
		ballPosition.x <= smallCubePosition.x + (sArea / 2.0f) && ballPosition.x> smallCubePosition.x - (sArea / 2.0f))
	{
		direction = reflect(direction, vec3(0.0f, 0.0f, 1.0f));
		ballRotation = cross(direction, vec3(0.0f, 0.0f, 1.0f));
	}
	/*hátulról*/
	if (abs(distPointPlane(ballPosition, smallCubePosition + vec3(0.0f, 0.0f, -(sArea / 2.0f)), vec3(0.0f, 0.0f, -1.0f))) < radius &&
		distPointPlane(ballPosition, smallCubePosition + vec3(0.0f, 0.0f, -(sArea / 2.0f)), vec3(0.0f, 0.0f, -1.0f))*distPointPlane(smallCubePosition, smallCubePosition + vec3(0.0f, 0.0f, -(sArea / 2.0f)), vec3(0.0f, 0.0f, -1.0f)) < 0 &&
		ballPosition.y <= smallCubePosition.y + (sArea / 2.0f) && ballPosition.y > smallCubePosition.y - (sArea / 2.0f) &&
		ballPosition.x <= smallCubePosition.x + (sArea / 2.0f) && ballPosition.x > smallCubePosition.x - (sArea / 2.0f))
	{
		direction = reflect(direction, vec3(0.0f, 0.0f, 1.0f));
		ballRotation = cross(direction, vec3(0.0f, 0.0f, 1.0f));
	}
	
	ballPosition += direction;
}

void init()
{
	compileShader();
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	cube = new Cube(area, false);
	smallCube = new Cube(sArea, true);
	sphere = new VBOSphere(radius, 360, 360);

	prog.use();
	for (int i = 0; i < 4; i++)
	{
		string str = "Spot[" + to_string(i) + "].intensity";
		const char *tmpPosition = str.c_str();
		prog.setUniform(tmpPosition, vec3(1.0f, 1.0f, 1.0f));
		str = "Spot[" + to_string(i) + "].exponent";
		tmpPosition = str.c_str();
		prog.setUniform(tmpPosition, 30.0f);
		str = "Spot[" + to_string(i) + "].cutoff";
		tmpPosition = str.c_str();
		prog.setUniform(tmpPosition, 5.0f);
	}

	prog.setUniform("LightIntensity", vec3(0.9f,0.9f,0.9f));

	cubeTexture = new Texture("src/texture/ice.tga", 0);
	ball = new Texture("src/texture/BeachBallTexture.tga", 0);
	smallCubeTexture = new Texture("src/texture/box.tga", 0);

	smallCubePosition = vec3(0.0f, -10.0f, 0.0f);
	ballPosition = vec3(0.0f);
	direction = vec3(0.01f, 0.03f, 0.1f);
	ballRotation = vec3(0.0f, 0.0f, 0.0f);
	ballAngle = 0.1f;
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

		prog.setUniform("LightPosition", vec4(0.0f)* camera.getView());

		prog.setUniform("Kd", 0.9f, 0.9f, 0.2f);
		prog.setUniform("Ks", 0.95f, 0.95f, 0.95f);
		prog.setUniform("Ka", 0.0f, 0.0f, 0.2f);
		prog.setUniform("Shininess", 100.0f);

		model = mat4(1.0f);
		setMatrices();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture->id);
		cube->render();

		vec4 lightPos[4];
		lightPos[0] = vec4(area / 2.0f, area / 2.0f, area / 2.0f, 1.0f);
		lightPos[1] = vec4(area / 2.0f, area / 2.0f, -area / 2.0f, 1.0f);
		lightPos[2] = vec4(-area / 2.0f, area / 2.0f, -area / 2.0f, 1.0f);
		lightPos[3] = vec4(-area / 2.0f, area / 2.0f, area / 2.0f, 1.0f);

		mat3 normalMatrix = mat3(vec3(camera.getView()[0]), vec3(camera.getView()[1]), vec3(camera.getView()[2]));
		for (int i = 0; i < 4; i++)
		{
			string str = "Spot[" + to_string(i) + "].position";
			const char *tmpPosition = str.c_str();
			prog.setUniform(tmpPosition, camera.getView() * lightPos[i]);
			str = "Spot[" + to_string(i) + "].direction";
			const char *tmpDirection = str.c_str();
			prog.setUniform(tmpDirection, normalMatrix * (smallCubePosition - vec3(lightPos[i])));
		}
		
		prog.setUniform("Kd", 0.9f, 0.9f, 0.3f);
		prog.setUniform("Ks", 0.95f, 0.95f, 0.95f);
		prog.setUniform("Ka", 0.0f, 0.0f, 0.0f);
		prog.setUniform("Shininess", 100.0f);

		moveBall();
		model = mat4(1.0f);
		ballAngle += 2.0f;
		model *= translate(ballPosition);
		model *= rotate(radians(ballAngle),ballRotation);
		setMatrices();

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, ball->id);
		sphere->render();

		model = mat4(1.0f);
		model *= translate(smallCubePosition);
		setMatrices();

		prog.setUniform("Kd", 0.7f, 0.8f, 0.3f);
		prog.setUniform("Ks", 0.95f, 0.95f, 0.95f);
		prog.setUniform("Ka", 0.0f, 0.0f, 0.0f);
		prog.setUniform("Shininess", 100.0f);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, smallCubeTexture->id);
		smallCube->render();

		camera.setCameraFront(ballPosition);
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