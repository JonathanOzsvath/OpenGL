#pragma once

#include<String>
#include <cookbookogl.h>

class GLShader
{

public:
    enum GLShaderType
    {
        VERTEX = GL_VERTEX_SHADER,
        FRAGMENT = GL_FRAGMENT_SHADER,
        GEOMETRY = GL_GEOMETRY_SHADER,
        TESS_CONTROL = GL_TESS_CONTROL_SHADER,
        TESS_EVALUATION = GL_TESS_EVALUATION_SHADER,
        COMPUTE = GL_COMPUTE_SHADER
    };

    GLShader(GLShaderType t);
    virtual ~GLShader();


    void readShader(const char *);
    void compileShader();

    GLuint getId();
private:
    std::string source;
    GLShaderType type;
    GLuint id;


};

