#pragma once
#include "Light.h"
class DirectionalLight :
    public Light
{
public:
	DirectionalLight();
	DirectionalLight(glm::vec3 col, GLfloat ambIntensity, GLfloat difIntensity, glm::vec3 difDir);
	~DirectionalLight();

	void UseLight(GLint colorLocation, GLint ambientIntensityLocation,
		GLint diffuseIntensityLocation, GLint diffuseDirectionLocation);

private:
    glm::vec3 direction;
};

