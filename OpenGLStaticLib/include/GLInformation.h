#pragma once

#include "cookbookogl.h"
#include <string>

void OpenGLInformation()
{
    //Get information from openGL

    const std::string vendor = (const char *)glGetString(GL_VENDOR);
    const GLubyte *render = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);
    const GLubyte *shadingLanguage = glGetString(GL_SHADING_LANGUAGE_VERSION);

    int major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    GLint numberOfExtension;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numberOfExtension);

    printf("OpenGL vendor: %s\n", vendor);
    printf("OpenGL version: %s -- %d.%d\n", version, major, minor);
    printf("OpenGL renderer: %s\n", render);
    printf("OpenGL shading language: %s\n", shadingLanguage);
    printf("OpenGL list of extensions:\n");

    for (GLint i = 0; i < numberOfExtension; i++)
    {
        printf("%s\n", glGetStringi(GL_EXTENSIONS, i));
    }
}
