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
#include "frustum.h"

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
mat4 projection1;

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

//shadow változók
int shadowMapWidth = 1024;
int shadowMapHeight = 1024;
GLuint depthTex;
GLuint shadowFBO;
GLuint pass1Index;
GLuint pass2Index;
mat4 shadowBias;
Frustum * lightFrustum;
mat4 lightPV;

//Texture
GLuint fal, vetito;

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
	prog.setUniform("Model", model);
	mat4 mv = view * model;
	prog.setUniform("ModelView", mv);
	prog.setUniform("NormalMatrix",
		glm::mat3(glm::vec3(mv[0]), glm::vec3(mv[1]), glm::vec3(mv[2])));
	mat4 mvp = projection * mv;
	prog.setUniform("MVP", mvp);
	prog.setUniform("ShadowMatrix", lightPV * model);
}

void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	width = w;
	height = h;
	projection = glm::perspective(glm::radians(60.0f), (float)w / h, 0.1f, 300.0f);
}

void setupFBO()
{
	GLfloat border[] = { 1.0f, 0.0f, 0.0f, 0.0f };

	glActiveTexture(GL_TEXTURE2);

	glGenTextures(1, &depthTex);
	glBindTexture(GL_TEXTURE_2D, depthTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, shadowMapWidth, shadowMapHeight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

	// Assign the depth buffer texture to texture channel 0
	glBindTexture(GL_TEXTURE_2D, depthTex);

	// Create and set up the FBO
	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D, depthTex, 0);

	GLenum drawBuffers[] = { GL_NONE };
	glDrawBuffers(1, drawBuffers);

	GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result == GL_FRAMEBUFFER_COMPLETE) {
		printf("Framebuffer is complete.\n");
	}
	else {
		printf("Framebuffer is not complete.\n");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, vetito);
	model = mat4(1.0f);
	model *= glm::translate(smallCubePosition);
	setMatrices();
	cube->render();

	prog.setUniform("Material.Kd", 0.7f, 0.7f, 0.7f);
	prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
	prog.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
	prog.setUniform("Material.Shininess", 100.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fal);
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
	glViewport(0, 0, width, height);

	cube = new VBOCube();
	plane = new VBOPlane(30.0f, 30.0f, 1, 1);

	smallCubePosition = vec3(0.0f, 0.0f, 0.0f);

	projection1 = mat4(1.0f);
	projection1 = glm::perspective(glm::radians(50.0f),
		static_cast<float>(width) / static_cast <float> (height), 0.1f, 300.0f);

	//fal texture
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &fal);
	fal = TGAIO::loadTex("src/texture/desk.tga");
	glBindTexture(GL_TEXTURE_2D, fal);

	//vetítõ textura
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &vetito);
	vetito = TGAIO::loadTex("src/texture/flower.tga");
	glBindTexture(GL_TEXTURE_2D, vetito);

	// Load texture file
	glActiveTexture(GL_TEXTURE1);
	//valtas = 0;
	string kepek = "src/texture/mese/Vuk_1.tga";
	const char* tmp = kepek.c_str();
	vetit = TGAIO::loadTex(tmp);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

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

	// Set up the framebuffer object
	setupFBO();

	GLuint programHandle = prog.getProgramID();
	pass1Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "recordDepth");
	pass2Index = glGetSubroutineIndex(programHandle, GL_FRAGMENT_SHADER, "shadeWithShadow");

	shadowBias = mat4(vec4(0.5f, 0.0f, 0.0f, 0.0f),
		vec4(0.0f, 0.5f, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, 0.5f, 0.0f),
		vec4(0.5f, 0.5f, 0.5f, 1.0f)
		);

	lightFrustum = new Frustum(Projection::PERSPECTIVE);
	vec3 lightPos = projPos;  // World coords
	lightFrustum->orient(lightPos, projAt, vec3(0.0f, -1.0f, 0.0f));
	lightFrustum->setPerspective(24.2f, 1.0f, 1.0f, 50.0f);
	lightPV = shadowBias * lightFrustum->getProjectionMatrix() * lightFrustum->getViewMatrix();

	glActiveTexture(GL_TEXTURE2);
	prog.setUniform("ShadowMap", depthTex);

	//világítás init
	prog.setUniform("Spot.Intensity", vec3(0.5f));
	prog.setUniform("Spot.exponent", 5.0f);
	prog.setUniform("Spot.alfa", 5.0f);
	prog.setUniform("Spot.beta", 5.0f);

	prog.setUniform("Light.Intensity", vec3(0.5f));
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

		// Pass 1 (shadow map generation)
		view = lightFrustum->getViewMatrix();
		projection = lightFrustum->getProjectionMatrix();
		glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, shadowMapWidth, shadowMapHeight);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass1Index);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		drawScene();
		glFlush();
		glFinish();

		// Pass 2 (render)
		view = camera.getView();
		//prog.setUniform("Light.Position", view * vec4(lightFrustum->getOrigin(), 1.0f));
		projection = projection1;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, width, height);
		glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, 1, &pass2Index);
		glDisable(GL_CULL_FACE);
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