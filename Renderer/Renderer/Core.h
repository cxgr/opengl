#pragma once

#include <iostream>
#include <SDL.h>
#include <glew.h>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <assimp/Importer.hpp>

#include "Constants.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "Material.h"
#include "ThreeDModel.h"
#include "Skybox.h"

class Core
{
public:
	Core();
	~Core();

	const Uint32 FPS = 60;
	const Uint32 FRAME_RATE = 1000 / FPS;
	const int WINDOW_WIDTH = 1600;
	const int WINDOW_HEIGHT = 900;
	const int CENTRE_X = WINDOW_WIDTH / 2;
	const int CENTRE_Y = WINDOW_HEIGHT / 2;

	const int SHADOW_RESOLUTION = 2048;

	const float DEG_2_RAD = glm::pi<float>() / 180.f;

	Uint32 lastFrame = 0;
	bool isRunning = true;

	SDL_Window* window;
	bool Run();
	bool Init();
	void Update(float deltaTime);
	void ProcessInputs(float deltaTime);

	void Render_Central();
	void Render_Pass_DirShadow(DirectionalLight* dl);
	void Render_Pass_OmniDirShadow(PointLight* pl);
	void Render_Pass_Main(glm::mat4 projection, glm::mat4 view);
	void Render_SceneObjects();

	void Cleanup();

private:

	Skybox skybox;
	std::vector<Mesh*> meshes;
	std::vector<Shader*> shaders;
	Shader dirShadowShader, omniShadowShader;
	Camera mainCam;
	glm::mat4 mtxProjection;

	bool lightOn = true;

	void LoadShaders();
	void CreateTestObjects();
};