#pragma once

#include "Light.h"
#include "OmniShadowMap.h"

class PointLight :
    public Light
{
public:
    PointLight();
    PointLight(glm::vec3 col, float ambIntensity, GLfloat difIntensity,
        glm::vec3 pos, GLfloat con, GLfloat lin, GLfloat exp,
        GLsizei shadowRes, GLfloat near, GLfloat far);
    ~PointLight();

    void UseLight(GLint ambientColorLocation, GLint ambientIntensityLocation,
        GLint diffuseIntensityLocation, GLint positionLocation,
        GLint constantLocation, GLint linearLocation, GLint exponentLocation);

    glm::vec3 GetLightPosition();
    std::vector<glm::mat4> GetLightTransform();
    GLfloat GetFarPlane();

protected:
    glm::vec3 position;
    GLfloat constant, linear, exponent;

    GLfloat farPlane;
};

