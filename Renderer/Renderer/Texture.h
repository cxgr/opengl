#pragma once

#include <glew.h>
#include <iostream>
#include <stdexcept>
#include "Constants.h"

class Texture
{
public:
	Texture();
	Texture(const char* fileLoc);
	~Texture();

	bool LoadTexture(bool isTiled = false);
	void UseTexture();
	void ClearTexture();

private:
	GLuint texId;
	int wid, hei, bitDepth;
	const char* fileLocation;
};