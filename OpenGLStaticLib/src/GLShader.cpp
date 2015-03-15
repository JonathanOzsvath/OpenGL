#include "GLShader.h"
#include <fstream>
#include <sstream>
#include <iostream>


GLShader::GLShader(GLShaderType t) : type(t)
{
}

GLShader::~GLShader()
{
    glDeleteShader(id);
}

void GLShader::readShader(const char* load)
{
    std::ifstream infile(load, std::ios::in);
    if (!infile)
    {
        std::string message = std::string("unable to open: ") + load;
        exit(-1);
    }

    std::stringstream code;
    code << infile.rdbuf();
    infile.close();

    source = code.str();
}

void GLShader::compileShader()
{
    GLuint shader = glCreateShader(type);

    if (shader == 0)
    {
        std::cout << "Create shader error" << std::endl;
        exit(-1);
    }


    const char * c_code = source.c_str();
    glShaderSource(shader, 1, &c_code, NULL);

    glCompileShader(shader);

    GLint result;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE)
    {

        std::cout << "Error with compile shader" << std::endl;


        GLint loglen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &loglen);

        if (loglen > 0)
        {
            GLchar * logs = new GLchar[loglen];

            GLint written;
            glGetShaderInfoLog(shader, loglen, &written, logs);

            printf("Shader log:\n %s", logs);
        }
        //exit(-1);
    }
    else
    {
        id = shader;
    }
}

GLuint GLShader::getId()
{
    return id;
}