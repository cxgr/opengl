#pragma once

#include <glm/glm.hpp>
#include <glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "ShadowMap.h"

class Light
{
public:
	Light();
	Light(glm::vec3 col, GLfloat ambIntensity, GLfloat difIntensity, GLsizei shadowRes);

	ShadowMap* GetShadowMap();

	void SetAmbientIntensity(float amb);
	void SetDiffuseIntensity(float dif);

	~Light();

protected:
	glm::vec3 color;
	float ambientIntensity;
	GLfloat diffuseIntensity;

	glm::mat4 mtxLightProjection;

	ShadowMap* shadowMap;
};

