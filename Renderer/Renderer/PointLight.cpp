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
	glm::vec3 pos, GLfloat con, GLfloat lin, GLfloat exp, GLsizei shadowRes) :
	Light(col, ambIntensity, difIntensity, shadowRes),
	position(pos),
	constant(con),
	linear(lin),
	exponent(exp)
{
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
