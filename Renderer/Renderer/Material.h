#pragma once

#include <glew.h>

class Material
{
public:
	Material();
	Material(GLfloat specIntensity, GLfloat shine);
	~Material();

	void UseMaterial(GLint specIntensityLocation, GLint shineLocation);

private:
	GLfloat specularIntensity;
	GLfloat shininess;
};

