#pragma once
#include "Light.h"
#include "Constants.h"
class DirectionalLight :
    public Light
{
public:
	DirectionalLight();
	DirectionalLight(glm::vec3 col, GLfloat ambIntensity, GLfloat difIntensity, glm::vec3 difDir, GLsizei shadowRes);
	~DirectionalLight();

	void UseLight(GLint colorLocation, GLint ambientIntensityLocation,
		GLint diffuseIntensityLocation, GLint diffuseDirectionLocation);

	glm::mat4 GetLightTransform();

private:
    glm::vec3 direction;
};

