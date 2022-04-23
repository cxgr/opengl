#include "Core.h"

Core::Core()
{
}

Core::~Core()
{
}

bool Core::Run()
{
	try
	{
		Init();
	}
	catch (std::runtime_error& e)
	{
		std::cout << e.what() << std::endl;
		return false;
	}

	lastFrame = SDL_GetTicks64();
	while (isRunning)
	{
		auto elapsed = SDL_GetTicks() - lastFrame;
		if (elapsed < FRAME_RATE)
		{
			SDL_Delay(FRAME_RATE - elapsed);
		}

		const auto deltaTime = (SDL_GetTicks() - lastFrame) / 1000.0f;
		//std::cout << "deltaTime: " << deltaTime << std::endl;

		ProcessInputs(deltaTime);
		Update(deltaTime);
		Render_Central();

		lastFrame = SDL_GetTicks64();
	}

	return true;
}

bool Core::Init()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		throw std::runtime_error(SDL_GetError());

	window = SDL_CreateWindow
	(
		"OpenGL",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_OPENGL
	);

	if (!window)
		throw std::runtime_error("SDL Windows initialization failed");

	SDL_GLContext openGLContext = SDL_GL_CreateContext(window);
	if (!openGLContext)
		throw std::runtime_error(SDL_GetError());

	glewExperimental = GL_TRUE;
	GLenum error = glewInit();
	if (error != GLEW_OK)
	{
		SDL_DestroyWindow(window);
		SDL_GL_DeleteContext(openGLContext);
		throw std::runtime_error("glewInit failed");
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetSwapInterval(1);

	glEnable(GL_DEPTH_TEST);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	glEnable(GL_CULL_FACE);

	SDL_CaptureMouse(SDL_TRUE);
	SDL_ShowCursor(SDL_DISABLE);

	auto aspect = (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT;
	mtxProjection = glm::perspective(glm::radians(60.f), aspect, NEAR_PLANE, FAR_PLANE);
	mainCam = Camera(glm::vec3(0.f, 0.f, -5.f), glm::vec3(0.f, 1.f, 0.f), 90.f, 0.f, 10.f, 10.f);

	LoadShaders();
	CreateTestObjects();

	SDL_WarpMouseInWindow(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

	return true;
}

GLint uniformModel = 0;
GLint uniformProj = 0;
GLint uniformView = 0;
GLint uniformEyePos = 0;

GLint unifSpecIntensity = 0;
GLint unifShininess = 0;

GLint uniformOmniLightPos = 0, uniformFarPlane = 0;

Texture texBrick;
Texture texDirt;
Texture texFloor;

Material matShiny;
Material matDull;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
GLint pointLightCount = 0;
SpotLight spotLights[MAX_SPOT_LIGHTS];
GLint spotLightCount = 0;

ThreeDModel model0, model1, sph0, sphX, sphY, sphZ;

void Core::Update(float deltaTime)
{
	auto flOffset = mainCam.GetPosition() + mainCam.GetUp() * -.4f;
	spotLights[0].UpdateTransform(flOffset, mainCam.GetForward());
}

void Core::ProcessInputs(float deltaTime)
{
	SDL_Event e;
	glm::vec2 mouseInput = glm::vec2(0.f);


	while (SDL_PollEvent(&e) != 0)
	{
		switch (e.type)
		{
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_ESCAPE)
			{
				isRunning = false;
				break;
			}
			break;

		case SDL_MOUSEMOTION:
			mouseInput.x = (float)e.motion.x - CENTRE_X;
			mouseInput.y = (float)e.motion.y - CENTRE_Y;
			SDL_WarpMouseInWindow(window, CENTRE_X, CENTRE_Y);
			break;
		}
	}

	auto keyStates = SDL_GetKeyboardState(NULL);

	glm::vec2 moveDir = glm::vec2(0.f);

	if (keyStates[SDL_SCANCODE_W])
		moveDir.y = 1.f;
	else if (keyStates[SDL_SCANCODE_S])
		moveDir.y = -1.f;

	if (keyStates[SDL_SCANCODE_D])
		moveDir.x = 1.f;
	else if (keyStates[SDL_SCANCODE_A])
		moveDir.x = -1.f;

	mainCam.HandleInput(deltaTime, moveDir, mouseInput);
}

void Core::Render_Central()
{
	glCullFace(GL_FRONT);
	Render_Pass_DirShadow(&mainLight);

	for (GLint i = 0; i < pointLightCount; ++i)
		Render_Pass_OmniDirShadow(&pointLights[i]);

	for (GLint i = 0; i < spotLightCount; ++i)
		Render_Pass_OmniDirShadow(&spotLights[i]);

	glCullFace(GL_BACK);
	Render_Pass_Main(mainCam.GetViewMatrix(), mtxProjection);

	glUseProgram(0);
	SDL_GL_SwapWindow(window);
}

void Core::Render_Pass_DirShadow(DirectionalLight* dl)
{
	dirShadowShader.UseShader();

	glViewport(0, 0, SHADOW_RESOLUTION, SHADOW_RESOLUTION);
	dl->GetShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	auto lightTransform = dl->GetLightTransform();
	dirShadowShader.SetDirLightTransform(&lightTransform);

	uniformModel = dirShadowShader.GetModelLocation();
	Render_SceneObjects();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Core::Render_Pass_OmniDirShadow(PointLight* pl)
{
	omniShadowShader.UseShader();

	glViewport(0, 0, SHADOW_RESOLUTION, SHADOW_RESOLUTION);
	pl->GetShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = omniShadowShader.GetModelLocation();

	uniformOmniLightPos = omniShadowShader.GetOmniLightPosLocation();
	glUniform3fv(uniformOmniLightPos, 1, glm::value_ptr(pl->GetLightPosition()));
	uniformFarPlane = omniShadowShader.GetFarPlaneLocation();
	glUniform1f(uniformFarPlane, pl->GetFarPlane());
	
	omniShadowShader.SetOmniLightTransforms(pl->GetLightTransform());

	Render_SceneObjects();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Core::Render_Pass_Main(glm::mat4 view, glm::mat4 projection)
{
	shaders[0]->UseShader();

	uniformModel = shaders[0]->GetModelLocation();
	uniformView = shaders[0]->GetViewLocation();
	uniformProj = shaders[0]->GetProjectionLocation();
	uniformEyePos = shaders[0]->GetCamPosLocation();

	unifSpecIntensity = shaders[0]->GetSpecIntensityLocation();
	unifShininess = shaders[0]->GetShininessLocation();

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	glClearColor(0.0f, 0.1f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(uniformProj, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform3fv(uniformEyePos, 1, glm::value_ptr(mainCam.GetPosition()));

	shaders[0]->SetDirectionalLight(&mainLight);
	//auto lightTransform = mainLight.GetLightTransform();
	shaders[0]->SetDirLightTransform(mainLight.GetLightTransform());
	shaders[0]->SetPointLights(pointLights, pointLightCount);
	shaders[0]->SetSpotLights(spotLights, spotLightCount);

	shaders[0]->SetTexture(0);
	mainLight.GetShadowMap()->Read(GL_TEXTURE1);
	shaders[0]->SetDirShadowMap(1);

	Render_SceneObjects();

}

void Core::Render_SceneObjects()
{
	glm::mat4 model(1.0f), tra, rot, scl;

	model = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -1.5f, 0.f));
	glUniformMatrix4fv(uniformModel, 1, false, glm::value_ptr(model));
	texFloor.UseTexture();
	matShiny.UseMaterial(unifSpecIntensity, unifShininess);
	//meshes[2]->RenderMesh();

	rot = glm::rotate(glm::mat4(1.f), glm::radians(180.f), glm::vec3(1.f, 0.f, 0.f));
	model = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 4.5f, 0.f)) * rot;
	glUniformMatrix4fv(uniformModel, 1, false, glm::value_ptr(model));
	texFloor.UseTexture();
	matShiny.UseMaterial(unifSpecIntensity, unifShininess);
	meshes[3]->RenderMesh();


	tra = glm::translate(glm::mat4(1.f), glm::vec3(-8.f, 0.f, 6.f));
	scl = glm::scale(glm::mat4(1.f), glm::vec3(0.008f));
	model = tra * scl * glm::mat4(1.f);
	glUniformMatrix4fv(uniformModel, 1, false, glm::value_ptr(model));
	model0.Render();


	tra = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 6.f));
	scl = glm::scale(glm::mat4(1.f), glm::vec3(.3f));
	rot = glm::rotate(glm::mat4(1.f), glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
	model = tra * rot * scl * glm::mat4(1.f);
	glUniformMatrix4fv(uniformModel, 1, false, glm::value_ptr(model));
	model1.Render();

	model = glm::mat4(1.f);
	scl = glm::scale(glm::mat4(1.f), glm::vec3(.1f));
	model = scl * model;
	glUniformMatrix4fv(uniformModel, 1, false, glm::value_ptr(model));
	sph0.Render();

	model = glm::mat4(1.f);
	tra = glm::translate(glm::mat4(1.f), glm::vec3(3.f, 0.f, 0.f));
	scl = glm::scale(glm::mat4(1.f), glm::vec3(.1f));
	model = tra * scl * model;
	glUniformMatrix4fv(uniformModel, 1, false, glm::value_ptr(model));
	sphX.Render();

	model = glm::mat4(1.f);
	tra = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 2.f, 0.f));
	scl = glm::scale(glm::mat4(1.f), glm::vec3(.1f));
	model = tra * scl * model;
	glUniformMatrix4fv(uniformModel, 1, false, glm::value_ptr(model));
	sphY.Render();

	model = glm::mat4(1.f);
	tra = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 2.f));
	scl = glm::scale(glm::mat4(1.f), glm::vec3(.1f));
	model = tra * scl * model;
	glUniformMatrix4fv(uniformModel, 1, false, glm::value_ptr(model));
	sphZ.Render();
}

void Core::Cleanup()
{
	for (const auto& m : meshes)
		m->ClearMesh();
	SDL_Quit();
}

void Core::LoadShaders()
{
	Shader* s1 = new Shader();
	std::cout << "loading phong" << std::endl;
	s1->CreateFromFiles("Shaders/Phong.vert", nullptr, "Shaders/Phong.frag");
	shaders.push_back(s1);

	std::cout << "loading dshadow" << std::endl;
	dirShadowShader.CreateFromFiles("Shaders/DirectionalShadowMap.vert");

	std::cout << "loading oshadow" << std::endl;
	omniShadowShader.CreateFromFiles("Shaders/OmniShadowMap.vert", "Shaders/OmniShadowMap.geom", "Shaders/OmniShadowMap.frag");
}

void Core::CreateTestObjects()
{
	std::vector<unsigned> indices =
	{
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	std::vector<GLfloat> verts =
	{
		-1.f, -1.f, -.6f, //xyz
		0.f, 0.f, //uv
		0.f, 0.f, 0.f, //nrm

		0.f, -1.f, 1.f,
		0.5f, 0.f,
		0.f, 0.f, 0.f,

		1.f, -1.f, -0.6f,
		1.f, 0.f,
		0.f, 0.f, 0.f,

		0.f, 1.f, 0.f,
		0.5f, 1.0f,
		0.f, 0.f, 0.f
	};

	std::vector<GLfloat> floorVerts =
	{
		-30.f, 0.f, -30.f,
		0.f, 0.f,
		0.f, 1.f, 0.f,

		30.f, 0.f, -30.f,
		10.f, 0.f,
		0.f, 1.f, 0.f,

		-30.f, 0.f, 30.f,
		0.f, 10.f,
		0.f, 1.f, 0.f,

		30.f, 0.f, 30.f,
		10.f, 10.f,
		0.f, 1.f, 0.f
	};

	std::vector<unsigned> floorIndices =
	{
		0, 2, 1,
		1, 2, 3
	};

	Mesh::CalculateAvgNormals(indices.data(), indices.size(), verts.data(), verts.size(), 8, 5);

	Mesh* m1 = new Mesh();
	m1->CreateMesh(verts.data(), verts.size(), indices.data(), indices.size());
	meshes.push_back(m1);

	Mesh* m2 = new Mesh();
	m2->CreateMesh(verts.data(), verts.size(), indices.data(), indices.size());
	meshes.push_back(m2);

	Mesh* floorObj = new Mesh();
	floorObj->CreateMesh(floorVerts.data(), floorVerts.size(), floorIndices.data(), floorIndices.size());
	meshes.push_back(floorObj);

	Mesh* ceilObj = new Mesh();
	ceilObj->CreateMesh(floorVerts.data(), floorVerts.size(), floorIndices.data(), floorIndices.size());
	meshes.push_back(floorObj);


	texBrick = Texture("Assets/brick.png");
	texBrick.LoadTexture(false);
	texDirt = Texture("Assets/dirt.png");
	texDirt.LoadTexture(false);
	//texFloor = Texture("Assets/blank.png");
	texFloor = Texture("Assets/dirt.png");
	texFloor.LoadTexture(true);

	matShiny = Material(1.f, 32);
	matDull = Material(.3f, 4);

	mainLight = DirectionalLight(glm::vec3(1.f, 1.f, 1.f), 0.15f, .75f,
		glm::vec3(0.f, 1.f, -.23f), SHADOW_RESOLUTION);

	pointLights[0] = PointLight(glm::vec3(0.f, 1.f, 0.f), 0.f, .9f,
		glm::vec3(-4.f, 2.f, 0.f), .3f, .1f, .1f, SHADOW_RESOLUTION, NEAR_PLANE, FAR_PLANE);
	pointLightCount++;
	pointLights[1] = PointLight(glm::vec3(0.f, 0.f, 1.f), 0.f, .9f,
		glm::vec3(2.f, 1.f, 0.f), .3f, .2f, .1f, SHADOW_RESOLUTION, NEAR_PLANE, FAR_PLANE);
	pointLightCount++;
	pointLights[2] = PointLight(glm::vec3(1.f, .1f, .1f), 0.f, .9f,
		glm::vec3(.5f, 1.5f, -8.f), .3f, .2f, .1f, SHADOW_RESOLUTION, NEAR_PLANE, FAR_PLANE);
	pointLightCount++;

	spotLights[0] = SpotLight(glm::vec3(.75f, .85f, .5f), 0.f, 2.f,
		glm::vec3(5.f, 1.f, -6.f), glm::vec3(0.f, -1.f, 0.f), 1.f, 0.f, 0.f, 20.f, SHADOW_RESOLUTION, NEAR_PLANE, FAR_PLANE);
	spotLightCount++; 

	model0.LoadModel("Assets\\x-wing.obj");
	model1.LoadModel("Assets\\uh60.obj");
	sph0.LoadModel("Assets\\sphere.obj", true);
	sphX.LoadModel("Assets\\sphere.obj", true);
	sphY.LoadModel("Assets\\sphere.obj", true);
	sphZ.LoadModel("Assets\\sphere.obj", true);
}
