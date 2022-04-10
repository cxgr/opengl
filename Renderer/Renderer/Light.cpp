#include "Light.h"

Light::Light() : color(glm::vec3(1.f)), ambientIntensity(1.f), diffuseIntensity(0.f)
{
}

Light::Light(glm::vec3 col, GLfloat ambIntensity, GLfloat difIntensity) :
	color(col),
	ambientIntensity(ambIntensity),
	diffuseIntensity(difIntensity)
{
}

Light::~Light()
{
}
