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

	SDL_CaptureMouse(SDL_TRUE);
	SDL_ShowCursor(SDL_DISABLE);

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	mainCam = Camera(glm::vec3(0.f, 0.f, -5.f), glm::vec3(0.f, 1.f, 0.f), 90.f, 0.f, 10.f, 10.f);

	CreateTestObjects();

	SDL_WarpMouseInWindow(window, WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

	return true;
}

GLuint uniformModel, uniformProj, uniformView;

void Core::Update(float deltaTime)
{
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

	glm::mat4 model(1.0f);
	//auto tra = glm::translate(glm::mat4(1.f), glm::vec3(triOffset, 0.f, 0.f));
	auto tra = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, -3.f));
	//auto rot = glm::rotate(glm::mat4(1.f), angle * DEG_2_RAD, glm::vec3(0.f, 1.f, 0.f));
	auto scl = glm::scale(glm::mat4(1.f), glm::vec3(0.5f));
	//model = tra * rot * model;
	model = tra * model;
	
	auto aspect = (GLfloat)WINDOW_WIDTH / (GLfloat)WINDOW_HEIGHT;
	auto projMtx = glm::mat4(1.f);
	projMtx = glm::perspective(glm::radians(60.f), aspect, .001f, 100.f);

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(uniformProj, 1, GL_FALSE, glm::value_ptr(projMtx));
	auto view = mainCam.GetViewMatrix();
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));

	meshes[0]->RenderMesh();

	model = glm::translate(glm::mat4(1.f), glm::vec3(2.f, 0.f, -4.f));
	glUniformMatrix4fv(uniformModel, 1, false, glm::value_ptr(model));
	meshes[1]->RenderMesh();

	glUseProgram(0);

	///////////////////////////////////////////
	SDL_GL_SwapWindow(window);
}

void Core::Cleanup()
{
	SDL_Quit();
}

static const char* vShader = "Shaders/test.vert";

static const char* fShader = "Shaders/test.frag";

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
		-1.f, -1.f, 0.f,
		0.f, -1.f, 1.f,
		1.f, -1.f, 0.f,
		0.f, 1.f, 0.f
	};

	Mesh* m1 = new Mesh();
	m1->CreateMesh(verts.data(), verts.size(), indices.data(), indices.size());
	meshes.push_back(m1);

	Mesh* m2 = new Mesh();
	m2->CreateMesh(verts.data(), verts.size(), indices.data(), indices.size());
	meshes.push_back(m2);

	Shader* s1 = new Shader();
	//s1->CreateFromString(vShader, fShader);
	s1->CreateFromFiles(vShader, fShader);
	shaders.push_back(s1);

	uniformModel = shaders[0]->GetModelLocation();
	uniformProj = shaders[0]->GetProjectionLocation();
	uniformView = shaders[0]->GetViewLocation();
}