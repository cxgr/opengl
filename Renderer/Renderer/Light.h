#pragma once

#include <glm/glm.hpp>
#include <glew.h>
#include <glm/gtc/type_ptr.hpp>

class Light
{
public:
	Light();
	Light(glm::vec3 col, GLfloat ambIntensity, GLfloat difIntensity);
	~Light();

protected:
	glm::vec3 color;
	float ambientIntensity;
	GLfloat diffuseIntensity;
};

