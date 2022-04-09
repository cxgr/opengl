#include "Light.h"

Light::Light() : col(glm::vec3(1.f)), ambientIntensity(1.f)
{
}

Light::Light(glm::vec3 color, float ambIntensity) : col(color), ambientIntensity(ambIntensity)
{
}

Light::~Light()
{
}

void Light::UseLight(GLint ambientColorLocation, GLint ambientIntensityLocation)
{
	glUniform3fv(ambientColorLocation, 1, glm::value_ptr(col));
	glUniform1f(ambientIntensityLocation, ambientIntensity);
}
