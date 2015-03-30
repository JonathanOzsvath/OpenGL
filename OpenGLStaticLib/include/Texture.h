#pragma once

#include "tgaio.h"

class Texture
{
public:
	Texture(const char *);
	~Texture();

private:

};

Texture::Texture(const char * fname)
{
	GLint w, h;
	glActiveTexture(GL_TEXTURE0);
	GLubyte * data = TGAIO::read(fname, w, h);

	GLuint texID;
	glGenTextures(1, &texID);

	glBindTexture(GL_TEXTURE_2D, texID);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

Texture::~Texture()
{
}