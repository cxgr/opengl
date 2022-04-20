#pragma once
#include "Light.h"
class PointLight :
    public Light
{
public:
    PointLight();
    PointLight(glm::vec3 col, float ambIntensity, GLfloat difIntensity,
        glm::vec3 pos, GLfloat con, GLfloat lin, GLfloat exp, GLsizei shadowRes);
    ~PointLight();

    void UseLight(GLint ambientColorLocation, GLint ambientIntensityLocation,
        GLint diffuseIntensityLocation, GLint positionLocation,
        GLint constantLocation, GLint linearLocation, GLint exponentLocation);

protected:
    glm::vec3 position;
    GLfloat constant, linear, exponent;
};

