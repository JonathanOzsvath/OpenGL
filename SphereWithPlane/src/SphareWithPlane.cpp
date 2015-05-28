#include "GLShader.h"
#include "GLBuffer.h"
#include "GLProgram.h"
#include "GLFWProgram.h"
#include "vbosphere.h"
#include "vbomesh.h"
#include "vboplane.h"
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
VBOMesh *nyul;
VBOPlane *padlo, *fal1, *fal2, *vaszon;
VBOMesh *asztal, *computer, *sofa, *cupboard, *szek, *vetito;

/*vetítési változók*/
mat4 projProj;
mat4 projView;
vec3 projPos;
vec3 projAt;
vec3 projUp;
mat4 projScaleTrans;
int vetit;
vector<GLuint> kepek;
int valt;

//mozgatható model változó
vec3 nyulPosition;

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
GLuint padloTexture, fal;

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

void diaLoader()
{
	valt = 0;
	glActiveTexture(GL_TEXTURE1);
	for (int i = 1; i <= 5; i++)
	{
		GLuint tmp;
		glGenTextures(1, &tmp);
		string tmp2 = "src/texture/mese/Vuk_" + to_string(i) + ".tga";
		const char* tmp3 = tmp2.c_str();
		tmp = TGAIO::loadTex(tmp3);
		kepek.push_back(tmp);
		glBindTexture(GL_TEXTURE_2D, tmp);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	}
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, kepek[valt]);
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
	prog.setUniform("Light.Position", vec4(0.0f,0.0f,0.0f,1.0f) * camera.getView());

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

	prog.setUniform("texOff", true);
	model = mat4(1.0f);
	model *= scale(vec3(0.6f));
	model *= glm::translate(nyulPosition);
	setMatrices();
	nyul->render();

	model = mat4(1.0f);
	model *= scale(vec3(0.61f));
	model *= glm::translate(nyulPosition);
	setMatrices();
	nyul->render();

	prog.setUniform("texOff", false);

	prog.setUniform("Material.Kd", 0.7f, 0.7f, 0.7f);
	prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
	prog.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
	prog.setUniform("Material.Shininess", 100.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, padloTexture);
	model = mat4(1.0f);
	model *= translate(vec3(0.0f, -10.0f, 0.0f));
	setMatrices();
	padlo->render();

	prog.setUniform("texOff", true);
	model = mat4(1.0f);
	model *= translate(vec3(0.0f, 10.0f, 0.0f));
	setMatrices();
	padlo->render();
	prog.setUniform("texOff", false);

	prog.setUniform("Material.Kd", 0.7f, 0.7f, 0.7f);
	prog.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
	prog.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
	prog.setUniform("Material.Shininess", 100.0f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fal);
	model = mat4(1.0f);
	model *= translate(vec3(0.0f, 0.0f, 15.0f));
	model *= rotate(radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	setMatrices();
	fal1->render();

	model = mat4(1.0f);
	model *= translate(vec3(0.0f, 0.0f, -15.0f));
	model *= rotate(radians(90.0f), vec3(1.0f, 0.0f, 0.0f));
	setMatrices();
	fal1->render();

	model = mat4(1.0f);
	model *= translate(vec3(20.0f, 0.0f, 0.0f));
	model *= rotate(radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	setMatrices();
	fal2->render();

	model = mat4(1.0f);
	model *= translate(vec3(-20.0f, 0.0f, 0.0f));
	model *= rotate(radians(90.0f), vec3(0.0f, 0.0f, 1.0f));
	setMatrices();
	fal2->render();

	prog.setUniform("Material.Kd", 1.0f, 1.0f, 1.0f);
	prog.setUniform("Material.Ks", 1.0f, 1.0f, 1.0f);
	prog.setUniform("Material.Ka", 1.0f, 1.0f, 1.0f);
	prog.setUniform("Material.Shininess", 50.0f);

	prog.setUniform("texOff", true);
	model = mat4(1.0f);
	model *= scale(vec3(1.6f));
	model *= translate(vec3(-11.3f, -6.25f, -3.9f));
	setMatrices();
	asztal->render();

	prog.setUniform("Material.Ka", vec3(0.3f, 0.5f, 0.8f));
	prog.setUniform("Material.Kd", vec3(0.7f, 0.5f, 0.0f));
	prog.setUniform("Material.Ks", vec3(1.0f, 1.0f, 1.0f));
	model = mat4(1.0f);
	model *= translate(vec3(-11.5f, -6.95f, -13.9f));
	model *= scale(vec3(0.7f));
	setMatrices();
	computer->render();

	prog.setUniform("Material.Ka", vec3(0.56f, 0.32f, 0.25f));
	prog.setUniform("Material.Kd", vec3(0.56f, 0.32f, 0.25f));
	prog.setUniform("Material.Ks", vec3(0.7f, 0.5f, 0.03f));
	model = mat4(1.0f);
	model *= translate(vec3(17.3f, -10.25f, -0.9f));
	model *= rotate(radians(-90.0f), vec3(0.0f, 1.0f, 0.0f));
	model *= scale(vec3(2.5f));
	setMatrices();
	sofa->render();

	model = mat4(1.0f);
	model *= translate(vec3(0.8f, -10.25f, 12.9f));
	model *= rotate(radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
	model *= scale(vec3(2.5f));
	setMatrices();
	cupboard->render();

	prog.setUniform("Material.Ka", vec3(0.2f, 0.2f, 0.25f));
	prog.setUniform("Material.Kd", vec3(0.3f, 0.4f, 0.3f));
	prog.setUniform("Material.Ks", vec3(0.2f, 0.5f, 0.03f));
	model = mat4(1.0f);
	model *= translate(vec3(-11.3f, -10.0f, -3.9f));
	model *= scale(vec3(0.5f));
	setMatrices();
	szek->render();

	model = mat4(1.0f);
	model *= translate(projPos);
	model *= rotate(radians(90.0f), vec3(0.0f, 1.0f, 0.0f));
	model *= scale(vec3(0.5f));
	setMatrices();
	vetito->render();

	prog.setUniform("texOff", false);
}

void init()
{
	compileShader();
	glClearColor(0.0f, 1.0f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, width, height);

	nyul = new VBOMesh("src/bunny.obj");
	padlo = new VBOPlane(40.0f, 30.0f, 1, 1);
	fal1 = new VBOPlane(40.0f, 20.0f, 1, 1);
	fal2 = new VBOPlane(20.0f, 30.0f, 1, 1);
	asztal = new VBOMesh("src/desk.obj");
	computer = new VBOMesh("src/computer.obj");
	sofa = new VBOMesh("src/sofa.obj");
	cupboard = new VBOMesh("src/cupboard.obj");
	szek = new VBOMesh("src/szek.obj");
	vetito = new VBOMesh("src/vetito.obj");

	nyulPosition = vec3(0.0f, 0.0f, 0.0f);

	projection1 = mat4(1.0f);
	projection1 = glm::perspective(glm::radians(50.0f),
		static_cast<float>(width) / static_cast <float> (height), 0.1f, 300.0f);

	//padló texture
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &padloTexture);
	padloTexture = TGAIO::loadTex("src/texture/floor.tga");
	glBindTexture(GL_TEXTURE_2D, padloTexture);

	//fal texture
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &fal);
	fal = TGAIO::loadTex("src/texture/fal.tga");
	glBindTexture(GL_TEXTURE_2D, fal);

	diaLoader();

	/*vetítés init*/
	projPos = vec3(0.0f, 0.0f, 0.0f);
	projAt = vec3(0.0f, 0.0f, -15.0f);
	projUp = vec3(0.0f, -1.0f, 0.0f);
	
	lightFrustum = new Frustum(Projection::PERSPECTIVE);

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

	

	glActiveTexture(GL_TEXTURE2);
	prog.setUniform("ShadowMap", depthTex);

	//világítás init
	prog.setUniform("Spot.Intensity", vec3(0.5f));
	prog.setUniform("Spot.exponent", 25.0f);
	prog.setUniform("Spot.alfa", 18.0f);
	prog.setUniform("Spot.beta", 14.0f);

	prog.setUniform("Light.Intensity", vec3(0.7f));
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

		projView = glm::lookAt(projPos, projAt, projUp);
		projProj = glm::perspective(30.0f, 1.0f, 0.2f, 1000.0f);

		projScaleTrans = glm::translate(vec3(0.5f)) * glm::scale(vec3(2.0f));
		mat4 m = projScaleTrans * projProj * projView;
		// Set the uniform variable
		prog.setUniform("ProjectorMatrix", m);

		
		vec3 lightPos = projPos;  // World coords
		lightFrustum->orient(lightPos, projAt, vec3(0.0f, -1.0f, 0.0f));
		lightFrustum->setPerspective(24.2f, 1.0f, 1.0f, 50.0f);
		lightPV = shadowBias * lightFrustum->getProjectionMatrix() * lightFrustum->getViewMatrix();

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, kepek[valt]);

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

	GLfloat cubeSpeed = 0.1f;

	if (action == GLFW_REPEAT || action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_UP:
			nyulPosition += vec3(0.0f, cubeSpeed, 0.0f);
			break;
		case GLFW_KEY_DOWN:
			nyulPosition -= vec3(0.0f, cubeSpeed, 0.0f);
			break;
		case GLFW_KEY_RIGHT:
			nyulPosition += vec3(cubeSpeed, 0.0f, 0.0f);
			break;
		case GLFW_KEY_LEFT:
			nyulPosition -= vec3(cubeSpeed, 0.0f, 0.0f);
			break;
		case GLFW_KEY_SPACE:
			nyulPosition += vec3(0.0f, 0.0f, cubeSpeed);
			break;
		case GLFW_KEY_COMMA:
			nyulPosition -= vec3(0.0f, 0.0f, cubeSpeed);
			break;
		case GLFW_KEY_U:
			projAt += vec3(0.0f, cubeSpeed, 0.0f);
			break;
		case GLFW_KEY_J:
			projAt -= vec3(0.0f, cubeSpeed, 0.0f);
			break;
		case GLFW_KEY_K:
			projAt += vec3(cubeSpeed, 0.0f, 0.0f);
			break;
		case GLFW_KEY_H:
			projAt -= vec3(cubeSpeed, 0.0f, 0.0f);
			break;
		case GLFW_KEY_M:
			projAt += vec3(0.0f, 0.0f, cubeSpeed);
			break;
		case GLFW_KEY_N:
			projAt -= vec3(0.0f, 0.0f, cubeSpeed);
			break;
		case GLFW_KEY_P:
			if (valt < 29)
			{
				valt++;
				cout << valt << endl;
			}
			break;
		case GLFW_KEY_O:
			if (valt > 0)
			{
				valt--;
				cout << valt << endl;
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
	window = initWindow("negyzetes Spotlight", width, height);

	init();
	resize(width, height);

	glfwSetKeyCallback(window, keyFunction);
	glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	mainloop();

	return 0;
}