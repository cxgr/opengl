#pragma once

#include <glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 initPos, glm::vec3 initUp, GLfloat initYaw, GLfloat initPitch, GLfloat initSpdMov, GLfloat initSpdRot);

	void HandleInput(float deltaTime, glm::vec2 keyInput, glm::vec2 mouseInput);

	glm::mat4 GetViewMatrix();
	glm::vec3 GetPosition();
	glm::vec3 GetForward();
	glm::vec3 GetUp();

	~Camera();

private:
	glm::vec3 pos;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	GLfloat yaw;
	GLfloat pitch;

	GLfloat spdMov;
	GLfloat rotSpd;

	void Update();
};

