#include <cookbookogl.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "GLShader.h"
#include "GLProgram.h"
#include "GLInformation.h"
#include "GLBuffer.h"
#include <array>
#include "GLFWProgram.h"



typedef glm::mat4 MAT4;
typedef glm::vec3 VEC3;
typedef std::string String;

GLFWwindow *window;
GLBuffer buffer(GLBuffer::BufferType::STATIC_DRAW);
GLProgram fillShader;
GLProgram lineShader;


void CreateVBOs()
{


    float positionData[] =
    {

        0.0, 0.4, 0.0,
        -0.4, -0.4, 0.0,
        0.4, -0.4, 0.0,
        -0.2, 0.0, 0.0,
        0.2, 0.0, 0.0,
        0.0, -0.4, 0.0
    };
    //11 position data

    float colorData[] =
    {
        1.0, 1.0, 0.0, 0.3,
        1.0, 1.0, 0.0, 0.3,
        1.0, 1.0, 0.0, 0.3,
        1.0, 1.0, 0.0, 0.3,
        1.0, 1.0, 0.0, 0.3,
        1.0, 1.0, 0.0, 0.3
    };

    std::array<unsigned int, 9> indexData = {0,3,4, 1,5,3, 2,4,5 };

    buffer.setVertexBuff(positionData, sizeof(positionData)/sizeof(*positionData));
    buffer.setSizeOfFrag(4);
    buffer.setFragBuff(colorData, sizeof(colorData)/sizeof(*colorData));
    buffer.setIndexBuff(indexData.data(), indexData.size());
    buffer.genBuffer();
}

void initializeGL()
{
    glClearColor(1.0, 1.0, 1.0, 1.0);

    GLShader fragmentShader(GLShader::GLShaderType::FRAGMENT);
    fragmentShader.readShader("src/shader/basic.frag");
    fragmentShader.compileShader();

    GLShader vertexShader(GLShader::GLShaderType::VERTEX);
    vertexShader.readShader("src/shader/basic.vert");
    vertexShader.compileShader();

    fillShader.setShaders({ fragmentShader.getId(), vertexShader.getId() });
    fillShader.link();
    CreateVBOs();
	
    GLShader fragmentShader1(GLShader::GLShaderType::FRAGMENT);
    fragmentShader1.readShader("src/shader/basic1.frag");
    fragmentShader1.compileShader();

    GLShader vertexShader1(GLShader::GLShaderType::VERTEX);
    vertexShader1.readShader("src/shader/basic.vert");
    vertexShader1.compileShader();

    lineShader.setShaders({ fragmentShader1.getId(), vertexShader1.getId() });
    lineShader.link();
}


void mainLoop()
{
    while (!glfwWindowShouldClose(window) && !glfwGetKey(window, GLFW_KEY_ESCAPE))
    {

        /* check opengl error

         update scene

         render scene*/
        glClear(GL_COLOR_BUFFER_BIT);

        fillShader.use();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        //glBindVertexArray(vaoHandlers);
        buffer.use();
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);


        lineShader.use();
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}



int main(int argc, char *argv[])
{

    window = initWindow("Triangles", 1000, 800);


    // Initialization
    initializeGL();


    // Enter the main loop
    mainLoop();

    // Close window and terminate GLFW
    glfwTerminate();
    // Exit program
    exit(EXIT_SUCCESS);
}