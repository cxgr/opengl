#define STB_IMAGE_IMPLEMENTATION

#include "Texture.h"

#include <stdexcept>

Texture::Texture() : texId(0), wid(0), hei(0), bitDepth(0), fileLocation("")
{
}

Texture::Texture(const char* fileLoc) : texId(0), wid(0), hei(0), bitDepth(0), fileLocation(fileLoc)
{
}

Texture::~Texture()
{
	ClearTexture();
}

void Texture::LoadTexture()
{
	unsigned char* texData = stbi_load(fileLocation, &wid, &hei, &bitDepth, 0);
	if (!texData)
		throw std::runtime_error(std::string("failed to load texture: ").append(fileLocation));

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wid, hei, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(texData);
}

void Texture::UseTexture()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texId);
}

void Texture::ClearTexture()
{
	glDeleteTextures(1, &texId);
	texId = 0;
	wid = 0;
	hei = 0;
	bitDepth = 0;
	fileLocation = "";
}
