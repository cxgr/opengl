#include "DirectionalLight.h"

DirectionalLight::DirectionalLight() : Light(),
                                       direction(glm::vec3(0.f, -1.f, 0.f))
{
}

DirectionalLight::DirectionalLight(glm::vec3 col, GLfloat ambIntensity, GLfloat difIntensity, glm::vec3 difDir, GLsizei shadowRes) :
	Light(col, ambIntensity, difIntensity, shadowRes),
	direction(glm::normalize(difDir))
{
	mtxLightProjection = glm::ortho(-20.f, 20.f, -20.f, 20.f, NEAR_PLANE, FAR_PLANE);
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

glm::mat4 DirectionalLight::GetLightTransform()
{
	auto up = glm::vec3(0.f, 1.f, 0.f);
	if (1.f - abs(glm::dot(direction, glm::vec3(0.f, -1.f, 0.f))) <= FLT_EPSILON)
	{
		up = glm::vec3(0.f, 0.f, -1.f);
	}

	return mtxLightProjection * glm::lookAt(-direction, glm::vec3(0.f), up);
}
