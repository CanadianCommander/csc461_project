#include "Debug.h"
#include "Program.h"
#include "IO/ScreenCapture.h"

#ifndef NDEBUG

Program::Program(LogPriority logPriority, LogCategory logCategory)
{
	SetLogPriority(logPriority);
	SetLogCategory(logCategory);
#else
	Program::Program()
#endif
	InitializeSDL();
	InitializeOpenGL(); }

Program::~Program()
{
	SDL_GL_DeleteContext(_context);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

void Program::InitializeSDL()
{
	SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
	LogSDL("SDL_Init");

	_windowWidth = 640;
	_windowHeight = 480;
	_window = SDL_CreateWindow(
			"CSC 461",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			_windowWidth,
			_windowHeight,
			SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);
	LogSDL("SDL_CreateWindow");

	SDL_version sdlVersion;
	SDL_GetVersion(&sdlVersion);
	LogInfo(LogCategory::ALL, "SDL version: %d.%d.%d.", sdlVersion.major, sdlVersion.minor, sdlVersion.patch);
}

void Program::InitializeOpenGL()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	LogSDL("SDL_GL_SetAttribute");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	LogSDL("SDL_GL_SetAttribute");
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	LogSDL("SDL_GL_SetAttribute");
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	LogSDL("SDL_GL_SetAttribute");
	_context = SDL_GL_CreateContext(_window);
	LogSDL("SDL_GL_CreateContext");
	SDL_GL_SetSwapInterval(1);
	LogSDL("SDL_GL_SetSwapInterval");

	LogInfo(LogCategory::GRAPHICS, "OpenGL version: %s", glGetString(GL_VERSION));

	glGenVertexArrays(1, &_vertexArrayHandle);
	LogGL("glGenVertexArrays");
	glBindVertexArray(_vertexArrayHandle);
	LogGL("glBindVertexArray");

	glGenBuffers(1, &_vertexBufferHandle);
	LogGL("glGenBuffers");

	GLfloat vertices[] = {
			-1, 1, 0, 0, // Top-left
			1, 1, 1, 0, // Top-right
			1, -1, 1, 1, // Bottom-right
			-1, -1, 0, 1  // Bottom-left
	};
	glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferHandle);
	LogGL("glBindBuffer");
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	LogGL("glBufferData");

	/*
	*  TL    TR
	*   0----1
	*   |\   |
	*   | \  |
	*   |  \ |
	*   |   \|
	*   3----2
	*  BL    BR
	*/

	glGenBuffers(1, &_indexBufferHandle);
	LogGL("glGenBuffers");

	GLubyte indices[] = {
			0, 1, 2,
			2, 3, 0
	};

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferHandle);
	LogGL("glBindBuffer");
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	LogGL("glBufferData");

	const char* vertexShaderSource =

#include "Graphics/Shaders/textureShader.vs.glsl"
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	LogGL("glCreateShader");
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
	LogGL("glCompileSource");
	glCompileShader(vertexShader);
	LogGL("glCompileShader");

	auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	LogGL("glCreateShader");
	const char* fragmentShaderSource =

#include "Graphics/Shaders/textureShader.fs.glsl"
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
	LogGL("glCompileSource");
	glCompileShader(fragmentShader);
	LogGL("glCompileShader");

	auto shaderProgram = glCreateProgram();
	LogGL("glCompileProgram");
	glAttachShader(shaderProgram, vertexShader);
	LogGL("glAttachShader");
	glAttachShader(shaderProgram, fragmentShader);
	LogGL("glAttachShader");
	glLinkProgram(shaderProgram);
	LogGL("glLinkProgram");
	glUseProgram(shaderProgram);
	LogGL("glUseProgram");

	auto vertexPositionAttributeLocation = static_cast<GLuint>(glGetAttribLocation(shaderProgram, "vertexPosition"));
	LogGL("glGetAttribLocation");
	glEnableVertexAttribArray(vertexPositionAttributeLocation);
	LogGL("glEnableVertexAttribArray");
	glVertexAttribPointer(vertexPositionAttributeLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
	LogGL("glVertexAttribPointer");

	auto vertexTextureCoordinateLocation = static_cast<GLuint>(glGetAttribLocation(shaderProgram,
	                                                                               "vertexTextureCoordinate"));
	LogGL("glGetAttribLocation");
	glEnableVertexAttribArray(vertexTextureCoordinateLocation);
	LogGL("glEnableVertexAttribArray");
	glVertexAttribPointer(vertexTextureCoordinateLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
	                      (void*)(2 * sizeof(GLfloat)));
	LogGL("glVertexAttribPointer");

	glActiveTexture(GL_TEXTURE0);
	LogGL("glActiveTexture");

	float textureData[] = {
			1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f
	};

  std::shared_ptr<IO::Image> img = _screenCapture.GetScreenFrameBuffer();

	_texture = new Graphics::Texture(*img.get());
	_texture->UploadData(img->GetRawDataPtr());

	auto textureSamplerUniformLocation = glGetUniformLocation(shaderProgram, "textureSampler");
	LogGL("glGetUniformLocation");
	glUniform1i(textureSamplerUniformLocation, 0);
	LogGL("glUniform1i");
}

void Program::Loop()
{
	while (!_isExiting)
	{
		Frame();
	}
}

void Program::Frame()
{
  UpdateTextures();
	HandleEvents();
	Draw();
}

void PrintEvent(const SDL_Event * event)
{
	if (event->type == SDL_WINDOWEVENT) {
		switch (event->window.event) {
			case SDL_WINDOWEVENT_SHOWN:
				SDL_Log("Window %d shown", event->window.windowID);
				break;
			case SDL_WINDOWEVENT_HIDDEN:
				SDL_Log("Window %d hidden", event->window.windowID);
				break;
			case SDL_WINDOWEVENT_EXPOSED:
				SDL_Log("Window %d exposed", event->window.windowID);
				break;
			case SDL_WINDOWEVENT_MOVED:
				SDL_Log("Window %d moved to %d,%d",
				        event->window.windowID, event->window.data1,
				        event->window.data2);
				break;
			case SDL_WINDOWEVENT_RESIZED:
				SDL_Log("Window %d resized to %dx%d",
				        event->window.windowID, event->window.data1,
				        event->window.data2);
				break;
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				SDL_Log("Window %d size changed to %dx%d",
				        event->window.windowID, event->window.data1,
				        event->window.data2);
				break;
			case SDL_WINDOWEVENT_MINIMIZED:
				SDL_Log("Window %d minimized", event->window.windowID);
				break;
			case SDL_WINDOWEVENT_MAXIMIZED:
				SDL_Log("Window %d maximized", event->window.windowID);
				break;
			case SDL_WINDOWEVENT_RESTORED:
				SDL_Log("Window %d restored", event->window.windowID);
				break;
			case SDL_WINDOWEVENT_ENTER:
				SDL_Log("Mouse entered window %d",
				        event->window.windowID);
				break;
			case SDL_WINDOWEVENT_LEAVE:
				SDL_Log("Mouse left window %d", event->window.windowID);
				break;
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				SDL_Log("Window %d gained keyboard focus",
				        event->window.windowID);
				break;
			case SDL_WINDOWEVENT_FOCUS_LOST:
				SDL_Log("Window %d lost keyboard focus",
				        event->window.windowID);
				break;
			case SDL_WINDOWEVENT_CLOSE:
				SDL_Log("Window %d closed", event->window.windowID);
				break;
#if SDL_VERSION_ATLEAST(2, 0, 5)
			case SDL_WINDOWEVENT_TAKE_FOCUS:
				SDL_Log("Window %d is offered a focus", event->window.windowID);
				break;
			case SDL_WINDOWEVENT_HIT_TEST:
				SDL_Log("Window %d has a special hit test", event->window.windowID);
				break;
#endif
			default:
				SDL_Log("Window %d got unknown event %d",
				        event->window.windowID, event->window.event);
				break;
		}
	}
}

void Program::HandleEvents()
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT)
		{
			_isExiting = true;
		}

		PrintEvent(&e);
	}
}

void Program::UpdateTextures(){
  std::shared_ptr<IO::Image> img = _screenCapture.GetScreenFrameBuffer();
  _texture->UploadData(img->GetRawDataPtr());
}

void Program::Draw()
{
	glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
  _texture->BindTexture();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);

	SDL_GL_SwapWindow(_window);
}