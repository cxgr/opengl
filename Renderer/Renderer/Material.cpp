#include "Material.h"

Material::Material() : specularIntensity(0.f), shininess(0.f)
{
}

Material::Material(GLfloat specIntensity, GLfloat shine) : specularIntensity(specIntensity), shininess(shine)
{
}

Material::~Material()
{
}

void Material::UseMaterial(GLint specIntensityLocation, GLint shineLocation)
{
	glUniform1f(specIntensityLocation, specularIntensity);
	glUniform1f(shineLocation, shininess);
}
