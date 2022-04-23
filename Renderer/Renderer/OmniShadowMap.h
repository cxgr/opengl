#pragma once
#include "ShadowMap.h"
class OmniShadowMap :
    public ShadowMap
{
public:
	OmniShadowMap();
	~OmniShadowMap();

	bool Init(GLsizei shadowRes);
	void Write();
	void Read(GLenum textureUnit);
};

