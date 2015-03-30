#include "GLShader.h"
#include "GLBuffer.h"
#include "GLProgram.h"
#include "GLFWProgram.h"
#include "vboplane.h"
#include "vbocube.h"
#include "vbosphere.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm\gtx\transform.hpp>
#include <stack>

using namespace glm;
using namespace std;

GLFWwindow *window;
GLuint width = 1920, height = 1080;
GLProgram prog;

VBOSphere *sphere;
VBOPlane *plane;

stack<mat4> modelStack;

mat4 model;
mat4 view;
mat4 projection;

vec4 worldLight = vec4(0.0f, 0.0f, 5.0f, 1.0f);

vec3 up = vec3(0.0f, 0.0f, 1.0f);
vec3 camPos = vec3(0.0f, 5.0f, 5.0f);
vec3 camLook = vec3(0.0f, 0.0f, 0.0f);

GLfloat mert1[20];
GLfloat mert2[20];
GLfloat mert3[20];

void compileShader()
{
	GLShader vert(GLShader::GLShaderType::VERTEX);
	vert.readShader("src/shader/phong.vert");
	vert.compileShader();

	GLShader frag(GLShader::GLShaderType::FRAGMENT);
	frag.readShader("src/shader/phong.frag");
	frag.compileShader();

	prog.setShaders({ vert.getId(), frag.getId() });
	prog.link();
	prog.use();
}

void setMatrices()
{
	mat4 mv = view * modelStack.top();
	modelStack.pop();
	prog.setUniform("ModelViewMatrix", mv);
	prog.setUniform("NormalMatrix",
		mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
	prog.setUniform("MVP", projection * mv);
}

void resizeGL(int w, int h) {
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.1f, 100.0f);
}

void init()
{
	compileShader();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	
	sphere = new VBOSphere(0.5f, 360, 360);
	plane = new VBOPlane(10,10,10,10);

	model = mat4(1.0f);
	

	modelStack.push(model);

	view = glm::lookAt(camPos, camLook, up);
	projection = mat4(1.0f);

	prog.setUniform("Light.Position", view * worldLight);
	prog.setUniform("Light.Intensity", 0.8f, 0.5f, 0.5f);

	for (size_t i = 0; i < 20; i++)
	{
		mert1[i] = (rand() % 10 - 5);
		mert2[i] = (rand() % 10 - 5);
		mert3[i] = (rand() % 10 - 5);
	}
}

void mainloop()
{
	while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		prog.setUniform("Light.Position", view * worldLight);

		prog.setUniform("Material.Kd", 0.2f, 0.8f, 0.3f);
		prog.setUniform("Material.Ka", 0.2f, 0.6f, 0.3f);
		prog.setUniform("Material.Ks", 0.2f, 0.2f, 0.8f);
		prog.setUniform("Material.Shininess", 100.0f);

		model = modelStack.top();
		glm::mat4 eredeti = model;
		//do stuff

		modelStack.push(model);
		setMatrices();

		sphere->render();
		//plane->render();
		for (size_t i = 0; i < 20; i++)
		{

			model = eredeti;

			model *= glm::translate(glm::vec3(mert1[i], mert2[i], mert3[i]));
			modelStack.push(model);
			setMatrices();
			sphere->render();
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void keyFunction(GLFWwindow *window, int key, int scanCode, int action, int mods)
{

	glm::vec3 viewRotVec = glm::vec3(0.0f);
	float viewRotAngle = 0.0f;

	switch (key)
	{
	case GLFW_KEY_UP:
		view = glm::rotate(view, -0.1f, glm::vec3(1.0, 0.0, 0.0));
		break;
	case GLFW_KEY_DOWN:
		view = glm::rotate(view, 0.1f, glm::vec3(1.0, 0.0, 0.0));
		break;
	case GLFW_KEY_LEFT:
		view = glm::rotate(view, 0.1f, glm::vec3(0.0, 0.0, 1.0));
		break;
	case GLFW_KEY_RIGHT:
		view = glm::rotate(view, -0.1f, glm::vec3(0.0, 0.0, 1.0));
		break;
	case GLFW_KEY_S:
		view = glm::translate(view, glm::vec3(0.0, 0.1, 0.0));
		break;
	case GLFW_KEY_A:
		view = glm::translate(view, glm::vec3(0.1, 0.0, 0.0));

		break;
	case GLFW_KEY_D:
		view = glm::translate(view, glm::vec3(-0.1, 0.0, 0.1));
		//move camera
		break;
	case GLFW_KEY_W:
		view = glm::translate(view, glm::vec3(0.0, -0.1, 0.0));
		//move camera
		break;
	case GLFW_KEY_R:
		for (size_t i = 0; i < 20; i++)
		{
			mert1[i] = (rand() % 10 - 5);
			mert2[i] = (rand() % 10 - 5);
			mert3[i] = (rand() % 10 - 5);
		}
		break;
	default:
		printf("Bad key :(\n");
		break;
	}



	glfwPollEvents();
}

int main()
{
	window = initWindow("RandomSphare", width, height);

	glfwSetKeyCallback(window, keyFunction);

	init();
	resizeGL(width, height);

	mainloop();

	return 0;
}