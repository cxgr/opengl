#pragma once

#include <iostream>
#include <SDL.h>
#include <glew.h>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"

class Core
{
public:
	Core();
	~Core();

	const Uint32 FPS = 60;
	const Uint32 FRAME_RATE = 1000 / FPS;
	const int WINDOW_WIDTH = 800;
	const int WINDOW_HEIGHT = 600;
	const int CENTRE_X = WINDOW_WIDTH / 2;
	const int CENTRE_Y = WINDOW_HEIGHT / 2;

	const float DEG_2_RAD = glm::pi<float>() / 180.f;

	Uint32 lastFrame = 0;
	bool isRunning = true;

	SDL_Window* window;
	bool Run();
	bool Init();
	void Update(float deltaTime);
	void ProcessInputs(float deltaTime);
	void Render();
	void Cleanup();

private:

	std::vector<Mesh*> meshes;
	std::vector<Shader*> shaders;
	Camera mainCam;

	void CreateTestObjects();
};