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
	catch (std::runtime_error &e)
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
		Render();

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
	glCullFace(GL_FRONT);

	SDL_CaptureMouse(SDL_TRUE);
	SDL_ShowCursor(SDL_DISABLE);

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	mainCam = Camera(glm::vec3(0.f, 0.f, -5.f), glm::vec3(0.f, 1.f, 0.f), 90.f, 0.f, 10.f, 10.f);

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

Texture texBrick;
Texture texDirt;
Texture texFloor;

Material matShiny;
Material matDull;

static const char* vShader = "Shaders/test.vert";
static const char* fShader = "Shaders/test.frag";

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
GLint pointLightCount = 0;
SpotLight spotLights[MAX_SPOT_LIGHTS];
GLint spotLightCount = 0;

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

void Core::Render()
{
	glClearColor(0.0f, 0.1f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	///////////////////////////////////////////

	shaders[0]->UseShader();

	uniformModel = shaders[0]->GetModelLocation();
	uniformProj = shaders[0]->GetProjectionLocation();
	uniformView = shaders[0]->GetViewLocation();
	uniformEyePos = shaders[0]->GetCamPosLocation();

	unifSpecIntensity = shaders[0]->GetSpecIntensityLocation();
	unifShininess = shaders[0]->GetShininessLocation();


	shaders[0]->SetDirectionalLight(&mainLight);
	shaders[0]->SetPointLights(pointLights, pointLightCount);
	shaders[0]->SetSpotLights(spotLights, 1);

	glm::mat4 model(1.0f);
	//auto tra = glm::translate(glm::mat4(1.f), glm::vec3(triOffset, 0.f, 0.f));
	auto tra = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -3.f));
	auto rot = glm::rotate(glm::mat4(1.f), 3.14f, glm::vec3(0.f, 1.f, 0.f));
	auto scl = glm::scale(glm::mat4(1.f), glm::vec3(0.5f));
	//model = tra * rot * model;
	model = tra * rot * model;
	
	auto aspect = (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT;
	auto projMtx = glm::mat4(1.f);
	projMtx = glm::perspective(glm::radians(60.f), aspect, .001f, 100.f);

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(uniformProj, 1, GL_FALSE, glm::value_ptr(projMtx));
	auto view = mainCam.GetViewMatrix();
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
	glUniform3fv(uniformEyePos, 1, glm::value_ptr(mainCam.GetPosition()));

	texBrick.UseTexture();
	matShiny.UseMaterial(unifSpecIntensity, unifShininess);
	meshes[0]->RenderMesh();

	model = glm::translate(glm::mat4(1.f), glm::vec3(2.f, 0.f, -4.f));
	glUniformMatrix4fv(uniformModel, 1, false, glm::value_ptr(model));

	texDirt.UseTexture();
	matDull.UseMaterial(unifSpecIntensity, unifShininess);
	meshes[1]->RenderMesh();

	model = glm::translate(glm::mat4(1.f), glm::vec3(0.f, -1.5f, 0.f));
	glUniformMatrix4fv(uniformModel, 1, false, glm::value_ptr(model));
	texFloor.UseTexture();
	matShiny.UseMaterial(unifSpecIntensity, unifShininess);
	meshes[2]->RenderMesh();

	glUseProgram(0);

	///////////////////////////////////////////
	SDL_GL_SwapWindow(window);
}

void Core::Cleanup()
{
	SDL_Quit();
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
		-10.f, 0.f, -10.f,
		0.f, 0.f,
		0.f, 1.f, 0.f,

		10.f, 0.f, -10.f,
		10.f, 0.f,
		0.f, 1.f, 0.f,

		-10.f, 0.f, 10.f,
		0.f, 10.f,
		0.f, 1.f, 0.f,

		10.f, 0.f, 10.f,
		10.f, 10.f,
		0.f, 1.f, 0.f
	};

	std::vector<unsigned> floorIndices =
	{
		0, 1, 2,
		1, 3, 2
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

	Shader* s1 = new Shader();
	s1->CreateFromFiles(vShader, fShader);
	shaders.push_back(s1);

	texBrick = Texture("Assets/brick.png");
	texBrick.LoadTexture(false);
	texDirt = Texture("Assets/dirt.png");
	texDirt.LoadTexture(false);
	//texFloor = Texture("Assets/plain.png");
	texFloor = Texture("Assets/dirt.png");
	texFloor.LoadTexture(true);

	matShiny = Material(1.f, 32);
	matDull = Material(.3f, 4);

	mainLight = DirectionalLight(glm::vec3(1.f, 1.f, 1.f), 0.f, .05f, glm::vec3(1.f, -1.f, 2.f));
	
	pointLights[0] = PointLight(glm::vec3(0.f, 1.f, 0.f), 0.f, 1.f,
		glm::vec3(-4.f, 2.f, 0.f), .3f, .1f, .1f);
	pointLightCount++;
	pointLights[1] = PointLight(glm::vec3(0.f, 0.f, 1.f), 0.f, 1.f,
		glm::vec3(2.f, 1.f, 0.f), .3f, .2f, .1f);
	pointLightCount++;
	pointLights[2] = PointLight(glm::vec3(1.f, .1f, .1f), 0.f, 1.f,
		glm::vec3(.5f, 1.5f, -8.f), .3f, .2f, .1f);
	pointLightCount++;

	spotLights[0] = SpotLight(glm::vec3(.75f, .85f, .5f), 0.f, 2.f,
		glm::vec3(5.f, 1.f, -6.f), glm::vec3(0.f, -1.f, 0.f), 1.f, 0.f, 0.f, 20.f);
	spotLightCount++;
}