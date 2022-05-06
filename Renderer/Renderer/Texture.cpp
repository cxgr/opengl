#define STB_IMAGE_IMPLEMENTATION

#include "Texture.h"

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

bool Texture::LoadTexture(bool isTiled)
{
	unsigned char* texData = stbi_load(fileLocation, &wid, &hei, &bitDepth, 4);
	if (!texData)
	{
		std::cout << "failed to load texture: " << fileLocation << std::endl;
		return false;
	}

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);

	auto wrapType = isTiled ? GL_REPEAT : GL_CLAMP_TO_EDGE;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapType);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapType);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, wid, hei, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(texData);

	return true;
}

//https://community.khronos.org/t/what-is-a-texture-unit/63250
void Texture::UseTexture()
{
	glActiveTexture(GL_TEXTURE1);
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
