#pragma once
#include <vector>
#include <cookbookogl.h>
//#include <GLShader.h>
#include <glm/glm.hpp>


class GLProgram
{
    GLuint program_id;
    std::vector<GLuint> shaders;
    void create();
    void attach();
    void linkin();
    void status();
public:
    GLProgram();
    GLProgram(std::vector<GLuint>);
    GLProgram(GLuint[]);
    //GLProgram(std::vector<GLShader>);
    //GLProgram(GLShader[]);
    virtual ~GLProgram();
    void link();
    void use();
    void setProgramID(GLuint);
    GLuint getProgramID();
    void setShaders(std::vector<GLuint>);
    void setShaders(GLuint[]);
    //void setShaders(std::vector<GLShader>);
    //void setShaders(GLShader[]);
    std::vector<GLuint> getShaders();

    void setUniform(const char *name, float x, float y, float z);
    void setUniform(const char *name, const glm::vec2 & v);
    void setUniform(const char *name, const glm::vec3 & v);
    void setUniform(const char *name, const glm::vec4 & v);
    void setUniform(const char *name, const glm::mat4 & m);
    void setUniform(const char *name, const glm::mat3 & m);
    void setUniform(const char *name, float val);
    void setUniform(const char *name, int val);
    void setUniform(const char *name, bool val);
    void setUniform(const char *name, GLuint val);


};

