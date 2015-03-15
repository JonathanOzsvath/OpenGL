#include "GLProgram.h"
#include <exception>

GLProgram::GLProgram()
{
}

GLProgram::GLProgram(std::vector<GLuint> shaders) : shaders(shaders)
{
}

GLProgram::GLProgram(GLuint shader[])
{
    for (size_t i = 0; i < sizeof(shader)/sizeof(*shader); i++)
    {
        shaders.push_back(shader[i]);
    }
}

//GLProgram::GLProgram(std::vector<GLShader> shader)
//{
//    for (auto s : shader)
//    {
//        shaders.push_back(s.getId());
//    }
//}
//
//GLProgram::GLProgram(GLShader shader[])
//{
//    for (size_t i = 0; i < sizeof(shader)/sizeof(*shader); i++)
//    {
//        shaders.push_back(shader[i].getId());
//    }
//}

GLProgram::~GLProgram()
{
    //glDeleteProgram(program_id);
}

void GLProgram::setProgramID(GLuint pid)
{
    program_id = pid;
}

GLuint GLProgram::getProgramID()
{
    return program_id;
}

void GLProgram::setShaders(std::vector<GLuint> shader)
{
    shaders = shader;
}

void GLProgram::setShaders(GLuint shader[])
{
    for (size_t i = 0; i < sizeof(shader)/sizeof(*shader); i++)
    {
        shaders.push_back(shader[i]);
    }
}

//void GLProgram::setShaders(std::vector<GLShader> shader)
//{
//    for (auto s : shader)
//    {
//        shaders.push_back(s.getId());
//    }
//}
//
//void GLProgram::setShaders(GLShader shader[])
//{
//    for (size_t i = 0; i < sizeof(shader) / sizeof(*shader); i++)
//    {
//        shaders.push_back(shader[i].getId());
//    }
//}

std::vector<GLuint> GLProgram::getShaders()
{
    return shaders;
}

void GLProgram::create()
{
    program_id = glCreateProgram();

    if (program_id == 0)
    {
        printf("error while creating program\n");
        exit(1);
    }
}

void GLProgram::attach()
{
    for (auto i : shaders)
    {
        glAttachShader(program_id, i);
    }
}

void GLProgram::linkin()
{
    glLinkProgram(program_id);
}

void GLProgram::status()
{
    GLint status = 0;
    glGetProgramiv(program_id, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        printf("problem with linking program\n");

        GLint loglen = 0;
        glGetProgramiv(program_id, GL_INFO_LOG_LENGTH, &loglen);

        GLchar * logs = new GLchar[loglen];
        GLint written = 0;
        glGetProgramInfoLog(program_id, loglen, &written, logs);

        printf("Program link status logs:\n %s", logs);
    }

}

void GLProgram::link()
{
    create();
    attach();
    linkin();
    status();
}

void GLProgram::use()
{
    glUseProgram(program_id);
}

void GLProgram::setUniform(const char *name, float x, float y, float z)
{
    GLuint loc = glGetUniformLocation(program_id, name);
    glUniform3f(loc, x, y, z);
}

void GLProgram::setUniform(const char *name, const glm::vec2 & v)
{
    GLuint loc = glGetUniformLocation(program_id, name);
    glUniform2f(loc, v.x, v.y);
}

void GLProgram::setUniform(const char *name, const glm::vec3 & v)
{
    GLuint loc = glGetUniformLocation(program_id, name);
    glUniform3f(loc, v.x, v.y, v.z);
}

void GLProgram::setUniform(const char *name, const glm::vec4 & v)
{
    GLuint loc = glGetUniformLocation(program_id, name);
    glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void GLProgram::setUniform(const char *name, const glm::mat4 & m)
{
    GLint loc = glGetUniformLocation(program_id, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, &m[0][0]);
}

void GLProgram::setUniform(const char *name, const glm::mat3 & m)
{
    GLuint loc = glGetUniformLocation(program_id, name);
    glUniformMatrix3fv(loc, 1, GL_FALSE, &m[0][0]);
}

void GLProgram::setUniform(const char *name, float val)
{
    GLuint loc = glGetUniformLocation(program_id, name);
    glUniform1f(loc, val);
}

void GLProgram::setUniform(const char *name, int val)
{
    GLuint loc = glGetUniformLocation(program_id, name);
    glUniform1i(loc, val);
}

void GLProgram::setUniform(const char *name, bool val)
{
    GLuint loc = glGetUniformLocation(program_id, name);
    glUniform1i(loc, val);
}

void GLProgram::setUniform(const char *name, GLuint val)
{
    GLuint loc = glGetUniformLocation(program_id, name);
    glUniform1ui(loc, val);
}
