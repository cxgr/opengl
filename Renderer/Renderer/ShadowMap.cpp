#include "ShadowMap.h"


ShadowMap::ShadowMap() : FBO(0), shadowMapTex(0)
{
}

ShadowMap::~ShadowMap()
{
	if (FBO)
		glDeleteFramebuffers(1, &FBO);
	if (shadowMapTex)
		glDeleteTextures(1, &shadowMapTex);
}

bool ShadowMap::Init(GLsizei shadowRes)
{
	shadowResolution = shadowRes;

	glGenFramebuffers(1, &FBO);
	glGenTextures(1, &shadowMapTex);
	glBindTexture(GL_TEXTURE_2D, shadowMapTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		shadowResolution, shadowResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	float whiteParam[] = { 1.f, 1.f, 1.f, 1.f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, whiteParam);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMapTex, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
		throw std::runtime_error("framebuffer error");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

void ShadowMap::Write()
{
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

void ShadowMap::Read(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, shadowMapTex);
}

GLsizei ShadowMap::GetShadowResolution()
{
	return shadowResolution;
}
