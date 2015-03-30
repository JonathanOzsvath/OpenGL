#pragma once

#include "tgaio.h"

using namespace std;

class CubeMapTexture
{
public:
	CubeMapTexture();
	CubeMapTexture(const char * fName,int size)
	{
		loadCubeMap(fName, size);
	}

	void loadCubeMap(const char * fName, int size)
	{
		glActiveTexture(GL_TEXTURE0);

		GLuint texID;
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

		const char * suffixes[] = { "posx", "negx", "posy", "negy", "posz", "negz" };
		GLuint targets[] = {
			GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
		};

		GLint w, h;
		// Allocate the cube map texture
		glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, size, size);

		// Load each cube-map face
		for (int i = 0; i < 6; i++) {
			string texName = string(fName) + "_" + suffixes[i] + ".tga";
			GLubyte * data = TGAIO::read(texName.c_str(), w, h);
			glTexSubImage2D(targets[i], 0, 0, 0, w, h,
				GL_RGBA, GL_UNSIGNED_BYTE, data);
			delete[] data;
		}
	}
};