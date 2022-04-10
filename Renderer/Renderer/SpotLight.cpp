#include "SpotLight.h"

SpotLight::SpotLight() : PointLight(),
                         direction(glm::vec3(0.f, -1.f, 0.f)),
                         edge(0.f),
                         processedEdge(cosf(glm::radians(edge)))
{
}

SpotLight::SpotLight(glm::vec3 col, float ambIntensity, GLfloat difIntensity, glm::vec3 pos, glm::vec3 dir, GLfloat con,
	GLfloat lin, GLfloat exp, GLfloat edg) :
	PointLight(col, ambIntensity, difIntensity, pos, con, lin, exp),
	direction(dir),
	edge(edg),
	processedEdge(cosf(glm::radians(edg)))
{
}

SpotLight::~SpotLight()
{
}

void SpotLight::UseLight(GLint ambientColorLocation, GLint ambientIntensityLocation, GLint diffuseIntensityLocation,
	GLint positionLocation, GLint directionLocation, GLint constantLocation, GLint linearLocation,
	GLint exponentLocation, GLint edgeLocation)
{
	glUniform3fv(ambientColorLocation, 1, glm::value_ptr(color));
	glUniform1f(ambientIntensityLocation, ambientIntensity);
	glUniform1f(diffuseIntensityLocation, diffuseIntensity);

	glUniform3fv(positionLocation, 1, glm::value_ptr(position));
	glUniform1f(constantLocation, constant);
	glUniform1f(linearLocation, linear);
	glUniform1f(exponentLocation, exponent);

	glUniform3fv(directionLocation, 1, glm::value_ptr(direction));
	glUniform1f(edgeLocation, processedEdge);

	posLocation = positionLocation;
	dirLocation = directionLocation;
}

void SpotLight::UpdateTransform(glm::vec3 pos, glm::vec3 dir)
{
	position = pos;
	direction = dir;
}
