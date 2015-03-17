#include "GLShader.h"
#include "GLProgram.h"
#include "GLBuffer.h"
#include "cookbookogl.h"
#include "GLFWProgram.h"
#include "vbotorus.h"
//a könyv shader feldolgozója
#include "glslprogram.h"
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "vbocube.h"
#include "vboplane.h"
#include "vbomesh.h"

GLFWwindow * window;
VBOTorus *torus;
VBOCube * cube;
VBOPlane * plane;
VBOMesh * mesh;
GLProgram shaderProgram;

mat4 view;
mat4 model;
mat4 projection;
GLfloat rotate = 0.0f;

GLboolean valtoztat = false;
float originalx, originaly;
vec3 moving(0.0f, 0.0f, 5.0f);

bool dragged = false;

void initializeGL()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);

	//mélység vizsgálat
    glEnable(GL_DEPTH_TEST);

	//hátsólap eldobása
    //glEnable(GL_CULL_FACE);

    torus = new VBOTorus(0.4f, 0.2f, 50, 50);
    cube = new VBOCube();
    plane = new VBOPlane(0.5f, 0.5f, 10, 10);
    mesh = new VBOMesh("src/untitled.obj");

    GLShader vertexShader(GLShader::GLShaderType::VERTEX);
    vertexShader.readShader("src/shader/basic.vert");
    vertexShader.compileShader();

    GLShader fragShader(GLShader::GLShaderType::FRAGMENT);
    fragShader.readShader("src/shader/basic.frag");
    fragShader.compileShader();

    shaderProgram.setShaders({ vertexShader.getId(), fragShader.getId() });
    shaderProgram.link();

    view = glm::lookAt(moving, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 2.0f, 0.0f));

    vec4 worldLight = vec4(5.0f, 5.0f, 2.0f, 1.0f);

    vec4 temp = view * worldLight;
    shaderProgram.setUniform("LightPosition", temp);
    GLuint loc = glGetUniformLocation(shaderProgram.getProgramID(), "LightPosition");
    glUniform4f(loc, temp.x, temp.y, temp.z, temp.w);



    model = mat4(1.0f);

    model *= glm::rotate(glm::radians(-35.0f), vec3(1.0f, 0.0f, 0.0f));
    model *= glm::rotate(glm::radians(35.0f), vec3(0.0f, 1.0f, 0.0f));

    projection = mat4(1.0f);
    glViewport(0, 0, 1600, 900);
    projection = glm::perspective(glm::radians(50.0f), (float)1366 / 720, 0.3f, 100.0f);

}

void mainLoop()
{
    while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        mat4 mv = view * model;
        shaderProgram.setUniform("ModelViewMatrix", mv);
        //GLint loc = glGetUniformLocation(shaderProgram.getProgramID(), "ModelViewMatrix");
        //glUniformMatrix4fv(loc, 1, GL_FALSE, &mv[0][0]);

        mat3 m2 = mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]));
        shaderProgram.setUniform("NormalMatrix", m2);
        //loc = glGetUniformLocation(shaderProgram.getProgramID(), "NormalMatrix");
        //glUniformMatrix3fv(loc, 1, GL_FALSE, &m2[0][0]);

        mat4 m = projection * mv;
        shaderProgram.setUniform("MVP", m);
        //loc = glGetUniformLocation(shaderProgram.getProgramID(), "MVP");
        //glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);

        if (valtoztat)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        shaderProgram.use();
        //torus->render();

        cube->render();

        //plane->render();

        //mesh->render();

        glfwSwapBuffers(window);
        glfwPollEvents();

    }
}



void mouseFunction(GLFWwindow *window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        valtoztat = !valtoztat;
    }

    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    glm::vec3 pos = glm::vec3(xpos, 800 - ypos, 0.0f);
    glm::vec4 viewportdata = glm::vec4(0.0f, 0.0f, 1000.0f, 800.0f);

    glm::mat4 view = glm::lookAt(vec3(0.0f, 0.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = mat4(1.0f);

    glm::vec3 un = glm::unProject(pos, view, projection, viewportdata);
    float xp = un[0], yp = un[1];

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        view = glm::lookAt(vec3(0.0f, 0.0f, 2.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 2.0f, 0.0f));
    }


    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
    {
        dragged = false;
    }


}

void keyFunction(GLFWwindow *window, int key, int scanCode, int action, int mods)
{
    if (action == GLFW_REPEAT)
    {
        switch (key)
        {
        case GLFW_KEY_W:
            moving.z -= 0.1f;
            break;
        case GLFW_KEY_S:
            moving.z += 0.1f;
            break;
        case GLFW_KEY_A:
            moving.x += 0.1f;
            break;
        case GLFW_KEY_D:
            moving.x -= 0.1f;
            break;
        case GLFW_KEY_Q:
            moving.y -= 0.1f;
            break;
        case GLFW_KEY_E:
            moving.y += 0.1f;
            break;
        default:
            printf("Bad key :(");
            break;
        }

        view = glm::lookAt(moving, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));

        glfwPollEvents();
    }
}


int main()
{
    window = initWindow("Donut", 1366, 720);

    initializeGL();

    glfwSetMouseButtonCallback(window, mouseFunction);
    glfwSetKeyCallback(window, keyFunction);

    mainLoop();

    return 0;
}

