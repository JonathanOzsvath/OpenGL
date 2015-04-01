#pragma once

#include "tgaio.h"

class Texture
{
public:
	Texture(const char *, int);
	~Texture();

	void loadTexture(const char *, int);
	GLuint id;

private:
};

Texture::Texture(const char * fName, int unit)
{
	loadTexture(fName, unit);
}

Texture::~Texture()
{
}

void Texture::loadTexture(const char *fName,int unit)
{
	GLint w, h;
	glActiveTexture(GL_TEXTURE0 );
	GLubyte * data = TGAIO::read(fName, w, h);

	glGenTextures(1, &this->id);

	glBindTexture(GL_TEXTURE_2D, this->id);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, w, h);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);*/
}