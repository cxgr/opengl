#include "Camera.h"

Camera::Camera()
{
}

Camera::Camera(glm::vec3 initPos, glm::vec3 initUp, GLfloat initYaw, GLfloat initPitch, GLfloat initSpdMov, GLfloat initSpdRot) :
	pos(initPos),
	front(glm::vec3(0.f, 0.f, -1.f)),
	worldUp(initUp),
	yaw(initYaw),
	pitch(initPitch),
	spdMov(initSpdMov),
	rotSpd(initSpdRot)
{
	Update();
}

void Camera::HandleInput(float deltaTime, glm::vec2 keyInput, glm::vec2 mouseInput)
{
	//std::cout << "keys" << keyInput.x << " " << keyInput.y << std::endl;
	//std::cout << "mouse" << mouseInput.x << " " << mouseInput.y << std::endl;

	yaw += rotSpd * mouseInput.x * deltaTime;
	pitch -= rotSpd * mouseInput.y * deltaTime;

	if (pitch > 89.f)
		pitch = 89.f;
	if (pitch < -89.f)
		pitch = -89.f;

	Update();


	if (abs(keyInput.x) > 0 || abs(keyInput.y) > 0)
	{
		auto velocity = glm::normalize(keyInput) * spdMov * deltaTime;
		pos += front * velocity.y;
		pos += right * velocity.x;
	}

	//std::cout << "pos" << pos.x << " " << pos.y << " " << pos.z << std::endl;
	//std::cout << "fwd" << front.x << " " << front.y << " " << front.z << std::endl;
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(pos, pos + front, up);
}

Camera::~Camera()
{
}

void Camera::Update()
{
	//std::cout << "yaw: " << yaw << "pitch: " << pitch << std::endl;

	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}
