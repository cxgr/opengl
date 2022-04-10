#pragma once

#include <glew.h>
#include "stb_image.h"

class Texture
{
public:
	Texture();
	Texture(char const* fileLoc);
	~Texture();

	void LoadTexture(bool isTiling);
	void UseTexture();
	void ClearTexture();

private:
	GLuint texId;
	int wid, hei, bitDepth;
	const char* fileLocation;
};