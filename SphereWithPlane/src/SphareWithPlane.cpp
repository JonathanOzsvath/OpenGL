#include "GLFWProgram.h"
#include "GLProgram.h"
#include "GLShader.h"
#include "vboplane.h"
#include "vbosphere.h"

GLFWwindow *window;
GLuint width = 1280, height = 720;
VBOPlane *plane;
VBOSphere *sphere;
GLProgram program;

void init()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);

	//plane = new VBOPlane(0.3f,0.4f,1,1);
	sphere = new VBOSphere(0.5f, 10, 10);

	GLShader vert(GLShader::GLShaderType::VERTEX);
	vert.readShader("src/shader/plane.vert");
	vert.compileShader();

	GLShader frag(GLShader::GLShaderType::FRAGMENT);
	frag.readShader("src/shader/plane.frag");
	frag.compileShader();

	program.setShaders({ vert.getId(), frag.getId() });
	program.link();
}

void mainloop()
{
	while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		program.use();
		//plane->render();
		sphere->render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

int main()
{
	window = initWindow("Pityunál", width, height);

	init();

	mainloop();

	return 0;
}