#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light(),
	direction(glm::vec3(0.f, -1.f, 0.f))
{
}

DirectionalLight::DirectionalLight(glm::vec3 col, GLfloat ambIntensity, GLfloat difIntensity, glm::vec3 difDir) :
	Light(col, ambIntensity, difIntensity),
	direction(difDir)
{
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::UseLight(GLint colorLocation, GLint ambientIntensityLocation,
	GLint diffuseIntensityLocation, GLint diffuseDirectionLocation)
{
	glUniform3fv(colorLocation, 1, glm::value_ptr(color));
	glUniform1f(ambientIntensityLocation, ambientIntensity);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);
	glUniform3fv(diffuseDirectionLocation, 1, glm::value_ptr(direction));
}
