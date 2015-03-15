#include "GLBuffer.h"
#include <iostream>


GLBuffer::GLBuffer(GLBuffer::BufferType dtype)
{
    drawType = dtype;
}


GLBuffer::~GLBuffer()
{
}

GLBuffer::GLBuffer(BufferType dtype, std::vector<float> vertexB)
{
    drawType = dtype;
    vertex = vertexB;
}

GLBuffer::GLBuffer(BufferType dtype, std::vector<float> vertexB, std::vector<float> fragB)
{
    drawType = dtype;
    vertex = vertexB;
    fragment = fragB;
}

GLBuffer::GLBuffer(BufferType dtype, std::vector<float> vertexB, std::vector<float> fragB, std::vector<unsigned int> indexB)
{
    drawType = dtype;
    vertex = vertexB;
    fragment = fragB;
    index = indexB;
}

GLBuffer::GLBuffer(BufferType dtype, float vertexB[])
{
    drawType = dtype;

    for (size_t i = 0; i < sizeof(vertexB) / sizeof(*vertexB); i++)
    {
        vertex.push_back(vertexB[i]);
    }
}

GLBuffer::GLBuffer(BufferType dtype, float vertexB[], float fragB[])
{
    drawType = dtype;
    for (size_t i = 0; i < sizeof(vertexB) / sizeof(*vertexB); i++)
    {
        vertex.push_back(vertexB[i]);
    }
    for (size_t i = 0; i < sizeof(fragB) / sizeof(*fragB); i++)
    {
        vertex.push_back(fragB[i]);
    }
}

GLBuffer::GLBuffer(BufferType dtype, float vertexB[], float fragB[], unsigned int indexB[])
{
    drawType = dtype;
    for (size_t i = 0; i < sizeof(vertexB) / sizeof(*vertexB); i++)
    {
        vertex.push_back(vertexB[i]);
    }
    for (size_t i = 0; i < sizeof(fragB) / sizeof(*fragB); i++)
    {
        vertex.push_back(fragB[i]);
    }
    for (size_t i = 0; i < sizeof(indexB) / sizeof(*indexB); i++)
    {
        vertex.push_back(indexB[i]);
    }
}

void GLBuffer::setSizeOfVertex(GLuint size)
{
    sizeOfVertex = size;
}

void GLBuffer::setSizeOfFrag(GLuint size)
{
    sizeOfFrag = size;
}

GLuint GLBuffer::getVaoHandlers() const
{
    return vaoHandlers;
}

void GLBuffer::genBuffer()
{
    GLuint size = 0;

    if (!vertex.empty())
    {
        size++;
    }

    if (!fragment.empty())
    {
        size++;
    }

    if (!index.empty())
    {
        size++;
    }

    sizeBuffer = size;
    glGenBuffers(size, vboHandlers);

    glGenVertexArrays(1, &vaoHandlers);
    glBindVertexArray(vaoHandlers);

    if (!vertex.empty())
    {
        attachVertex();
    }
    if (!fragment.empty())
    {
        attachFrag();
    }
    if (!index.empty())
    {
        attachIndex();
    }

}

void GLBuffer::setVertexBuff(float vertexB[], size_t size)
{

    for (size_t i = 0; i < size; i++)
    {
        vertex.push_back(vertexB[i]);
    }
}

void GLBuffer::setFragBuff(float fragB[], size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        fragment.push_back(fragB[i]);
    }
}

void GLBuffer::setIndexBuff(unsigned int indexB[], size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        index.push_back(indexB[i]);
    }
}

void GLBuffer::setVertexBuff(std::vector<float> vertexB)
{
    vertex = vertexB;
}

void GLBuffer::setFragBuff(std::vector<float> fragB)
{
    fragment = fragB;
}

void GLBuffer::setIndexBuff(std::vector<unsigned int> indexB)
{
    index = indexB;
}

void GLBuffer::attachVertex()
{
    glBindBuffer(GL_ARRAY_BUFFER, vboHandlers[0]);
    glBufferData(GL_ARRAY_BUFFER, vertex.size() * sizeof(float), &vertex[0], drawType);
    glVertexAttribPointer(0, sizeOfVertex, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
}

void GLBuffer::attachFrag()
{
    glBindBuffer(GL_ARRAY_BUFFER, vboHandlers[1]);
    glBufferData(GL_ARRAY_BUFFER, fragment.size() * sizeof(float), &fragment[0], drawType);
    glVertexAttribPointer(1, sizeOfFrag, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
}

void GLBuffer::attachIndex()
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandlers[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index.size() * sizeof(unsigned int), &index[0], drawType);
}

void GLBuffer::use()
{
    glBindVertexArray(vaoHandlers);
}