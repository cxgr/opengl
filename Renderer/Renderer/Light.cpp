#include "Light.h"

Light::Light() : color(glm::vec3(1.f)), ambientIntensity(1.f), diffuseIntensity(0.f)
{
}

Light::Light(glm::vec3 col, GLfloat ambIntensity, GLfloat difIntensity, GLsizei shadowRes) :
	color(col),
	ambientIntensity(ambIntensity),
	diffuseIntensity(difIntensity),
	mtxLightProjection(glm::mat4(1.f)),
	shadowMap(new ShadowMap())
{
	shadowMap->Init(shadowRes);
}

ShadowMap* Light::GetShadowMap()
{
	return shadowMap;
}

void Light::SetAmbientIntensity(float amb)
{
	ambientIntensity = amb;
}

void Light::SetDiffuseIntensity(float dif)
{
	diffuseIntensity = dif;
}

Light::~Light()
{
}
