#pragma once

#include <glew.h>
#include <stdexcept>

class ShadowMap
{
public:
	ShadowMap();
	~ShadowMap();

	virtual bool Init(GLsizei shadowRes);
	virtual void Write();
	virtual void Read(GLenum textureUnit);

	GLsizei GetShadowResolution();

	GLuint shadowMapTex;

protected:
	GLuint FBO;
	GLsizei shadowResolution;
};

