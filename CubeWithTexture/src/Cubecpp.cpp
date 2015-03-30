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
#include "Cube.h"
#include "tgaio.h"

using namespace glm;
using namespace std;

GLFWwindow *window;
GLProgram prog;
GLProgram prog2;
GLuint width = 1280, height = 720;

VBOPlane *plane;
VBOCube *cube2;
Cube *cube;

mat4 model;
mat4 view;
mat4 projection;
float angle;

vec4 worldLight = vec4(0.0f, 0.0f, 5.0f, 1.0f);

vec3 camPos = vec3(0.0f, 0.01f, 10.0f);
vec3 camLook = vec3(0.0f, 0.0f, 0.0f);
vec3 up = vec3(0.0f, 0.0f, 1.0f);

void compileShader()
{
	GLShader vert(GLShader::GLShaderType::VERTEX);
	vert.readShader("src/shader/cubemap_reflect.vert");
	vert.compileShader();

	GLShader frag(GLShader::GLShaderType::FRAGMENT);
	frag.readShader("src/shader/cubemap_reflect.frag");
	frag.compileShader();

	prog.setShaders({ vert.getId(), frag.getId() });
	prog.link();
}


void setMatrices()
{
	mat4 mv = view * model;
    prog.setUniform("ModelMatrix", model);
    prog.setUniform("MVP", projection * mv);
}

void setMatrices2()
{
	mat4 mv = view * model;
	prog.setUniform("ModelViewMatrix", mv);
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

void loadCubeMap(const char * baseFileName)
{
	glActiveTexture(GL_TEXTURE0);

	GLuint texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

	const char * suffixes[] = { "posx", "negx", "posy", "negy", "posz", "negz" };
	GLuint targets[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	GLint w, h;
	// Allocate the cube map texture
	glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, 256, 256);

	// Load each cube-map face
	for (int i = 0; i < 6; i++) {
		string texName = string(baseFileName) + "_" + suffixes[i] + ".tga";
		GLubyte * data = TGAIO::read(texName.c_str(), w, h);
		glTexSubImage2D(targets[i], 0, 0, 0, w, h,
			GL_RGBA, GL_UNSIGNED_BYTE, data);
		delete[] data;
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void init()
{
	compileShader();
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	cube = new Cube(50.0f, false);
	cube2 = new VBOCube();

	model = mat4(1.0f);


	view = glm::lookAt(camPos, camLook, up);
	angle = glm::radians(90.0);

	prog.setUniform("Kd", 0.9f, 0.5f, 0.3f);
	prog.setUniform("Ld", 1.0f, 1.0f, 1.0f);
	prog.setUniform("LightPosition", view * vec4(5.0f, 5.0f, 2.0f, 1.0f));

	//loadCubeMap("src/texture/citadella/night");
	//loadCubeMap("src/texture/Yokohama3/");
	loadCubeMap("src/texture/cubemap_night/night");
}

void mainloop()
{
	while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		model = mat4(1.0f);
		/*model *= scale(vec3(10.0));*/
	
		
		prog.use();
		prog.setUniform("WorldCameraPosition", view);

		prog.setUniform("DrawSkyBox", true);
		model = mat4(1.0f);
		setMatrices();
		cube->render();

		prog.setUniform("DrawSkyBox", false);

		prog.setUniform("MaterialColor", vec4(0.5f, 0.5f, 0.5f, 1.0f));
		prog.setUniform("ReflectFactor", 0.85f);

		
		setMatrices();
		cube2->render();
		
		/*setMatrices();
		cube->render();*/


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void keyFunction(GLFWwindow *window, int key, int scanCode, int action, int mods)
{

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
	default:
		printf("Bad key :(\n");
		break;
	}



	glfwPollEvents();
}

int main()
{
	window = initWindow("CubeWithTexture", width, height);

	init();
	resize(width, height);

	glfwSetKeyCallback(window, keyFunction);

	mainloop();

	return 0;
}