#pragma once

#include <glm/glm.hpp>
#include <glew.h>
#include <glm/gtc/type_ptr.hpp>

class Light
{
public:
	Light();
	Light(glm::vec3 color, float ambIntensity);
	~Light();

	void UseLight(GLint ambientColorLocation, GLint ambientIntensityLocation);

private:
	glm::vec3 col;
	float ambientIntensity;
};

