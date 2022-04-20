#pragma once
#include "PointLight.h"

class SpotLight :
    public PointLight
{
public:
    SpotLight();
    SpotLight(glm::vec3 col, float ambIntensity, GLfloat difIntensity,
        glm::vec3 pos, glm::vec3 dir,
        GLfloat con, GLfloat lin, GLfloat exp, GLfloat edg,
        GLsizei shadowRes);
    ~SpotLight();

    void UseLight(GLint ambientColorLocation, GLint ambientIntensityLocation,
        GLint diffuseIntensityLocation, GLint positionLocation, GLint directionLocation, 
        GLint constantLocation, GLint linearLocation, GLint exponentLocation, GLint edgeLocation);

    void UpdateTransform(glm::vec3 pos, glm::vec3 dir);

private:
    glm::vec3 direction;
    GLfloat edge, processedEdge;

    GLint posLocation = 0, dirLocation = 0;
};

