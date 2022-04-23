#include "PointLight.h"

PointLight::PointLight() :
	Light(),
	position(glm::vec3(0.f)),
	constant(1.f),
	linear(0.f),
	exponent(0.f)
{
}

PointLight::PointLight(glm::vec3 col, float ambIntensity, GLfloat difIntensity,
	glm::vec3 pos, GLfloat con, GLfloat lin, GLfloat exp,
	GLsizei shadowRes, GLfloat near, GLfloat far) :
	Light(col, ambIntensity, difIntensity, shadowRes),
	position(pos),
	constant(con),
	linear(lin),
	exponent(exp),
	farPlane(far)
{
	mtxLightProjection = glm::perspective(glm::radians(90.f), 1.f, near, far);

	shadowMap = new OmniShadowMap();
	shadowMap->Init(shadowRes);
}

PointLight::~PointLight()
{
}

void PointLight::UseLight(GLint ambientColorLocation, GLint ambientIntensityLocation, GLint diffuseIntensityLocation,
	GLint positionLocation, GLint constantLocation, GLint linearLocation, GLint exponentLocation)
{
	glUniform3fv(ambientColorLocation, 1, glm::value_ptr(color));
	glUniform1f(ambientIntensityLocation, ambientIntensity);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);

	glUniform3fv(positionLocation, 1, glm::value_ptr(position));
	glUniform1f(constantLocation, constant);
	glUniform1f(linearLocation, linear);
	glUniform1f(exponentLocation, exponent);
}

glm::vec3 PointLight::GetLightPosition()
{
	return position;
}

std::vector<glm::mat4> PointLight::GetLightTransform()
{
	std::vector<glm::mat4> lightTransforms;

	lightTransforms.push_back(mtxLightProjection * glm::lookAt(position, position + glm::vec3(1.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f)));
	lightTransforms.push_back(mtxLightProjection * glm::lookAt(position, position + glm::vec3(-1.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f)));

	lightTransforms.push_back(mtxLightProjection * glm::lookAt(position, position + glm::vec3(0.f, 1.f, 0.f), glm::vec3(0.f, 0.f, 1.f)));
	lightTransforms.push_back(mtxLightProjection * glm::lookAt(position, position + glm::vec3(0.f, -1.f, 0.f), glm::vec3(0.f, 0.f, -1.f)));

	lightTransforms.push_back(mtxLightProjection * glm::lookAt(position, position + glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f)));
	lightTransforms.push_back(mtxLightProjection * glm::lookAt(position, position + glm::vec3(0.f, 0.f, -1.f), glm::vec3(0.f, 1.f, 0.f)));

	return lightTransforms;
}

GLfloat PointLight::GetFarPlane()
{
	return farPlane;
}
