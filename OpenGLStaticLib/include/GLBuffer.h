#pragma once
#include<vector>
#include "cookbookogl.h"

class GLBuffer
{
public:
    enum BufferType
    {
        STREAM_DRAW = GL_STREAM_DRAW,
        STREAM_COPY = GL_STREAM_COPY,
        STREAM_READ = GL_STREAM_READ,
        STATIC_DRAW = GL_STATIC_DRAW,
        STATIC_COPY = GL_STATIC_COPY,
        STATIC_READ = GL_STATIC_READ,
        DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
        DYNAMIC_COPY = GL_DYNAMIC_COPY,
        DYNAMIC_READ = GL_DYNAMIC_READ
    };
    GLBuffer(BufferType);
    virtual ~GLBuffer();
    GLBuffer(BufferType, std::vector<float>);
    GLBuffer(BufferType, std::vector<float>, std::vector<float>);
    GLBuffer(BufferType, std::vector<float>, std::vector<float>, std::vector<unsigned int>);
    GLBuffer(BufferType, float[]);
    GLBuffer(BufferType, float[], float[]);
    GLBuffer(BufferType, float[], float[], unsigned int[]);
    void setSizeOfVertex(GLuint);
    void setSizeOfFrag(GLuint);
    GLuint getVaoHandlers() const;
    void genBuffer();
    void setVertexBuff(float[], size_t);
    void setFragBuff(float[], size_t);
    void setIndexBuff(unsigned int[], size_t);
    void setVertexBuff(std::vector<float>);
    void setFragBuff(std::vector<float>);
    void setIndexBuff(std::vector<unsigned int>);
    void use();
private:
    std::vector<float> vertex;
    std::vector<float> fragment;
    std::vector<unsigned int> index;
    GLuint vaoHandlers;
    GLuint vboHandlers[3];
    BufferType drawType;
    GLuint sizeOfVertex = 3;
    GLuint sizeOfFrag = 3;
    size_t sizeBuffer = 0;
    void attachVertex();
    void attachFrag();
    void attachIndex();

};

