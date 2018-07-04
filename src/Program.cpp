#include "Debug.h"
#include "Program.h"
#include "Codec/Transcoders/Open264Transcoder.h"
#include "Codec/Transcoders/VPXTranscoder.h"

#ifndef NDEBUG
Program::Program(LogPriority logPriority, LogCategory logCategory)
#else

Program::Program()
#endif
{
#ifndef NDEBUG
	SetLogPriority(logPriority);
	SetLogCategory(logCategory);
#endif
	InitializeSDL();
	InitializeOpenGL();
	InitializeNetwork();
	_isExiting = false;
	_transcoder = std::make_shared<Codec::VPXTranscoder>();
	_transcoder->InitEncoder();
	_transcoder->InitDecoder();
}

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

	std::shared_ptr<IO::Image> img = _screenCapture.GetScreenFrameBuffer();

	_texture = new Graphics::Texture(img->GetWidth(), img->GetHeight(), GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
	_texture->UploadData(img->GetRawDataPtr());

	auto textureSamplerUniformLocation = glGetUniformLocation(shaderProgram, "textureSampler");
	LogGL("glGetUniformLocation");
	glUniform1i(textureSamplerUniformLocation, 0);
	LogGL("glUniform1i");
}

void Program::InitializeNetwork()
{
	_socketHandle = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

void Program::Loop()
{
	_previousFrameTimePoint = hclock::now();

	while (!_isExiting)
	{
		RetryTick:
		auto currentFrameTimePoint = hclock::now();
		_accumulatedFrameTime += std::chrono::duration_cast<millisecondsf>(currentFrameTimePoint - _previousFrameTimePoint);
		_previousFrameTimePoint = currentFrameTimePoint;

		if (_isFixedUpdate && _accumulatedFrameTime < _targetElapsedTime)
		{
			auto sleepDuration = _targetElapsedTime - _accumulatedFrameTime;
			std::this_thread::sleep_for(sleepDuration);
			goto RetryTick;
		}

		if (_accumulatedFrameTime > _maximumElapsedTime) _accumulatedFrameTime = _maximumElapsedTime;

		Frame();
	}
}

void Program::Frame()
{
	if (_isFixedUpdate)
	{
		_frameTime = _targetElapsedTime;

		uint8_t stepCount = 0;
		while (!_isExiting && _accumulatedFrameTime >= _targetElapsedTime)
		{
			_accumulatedFrameTime -= _targetElapsedTime;
			stepCount++;
			Update();
		}

		if (stepCount == 1)
		{
			if (_frameLag > 0) _frameLag--;
		} else {
			_frameLag += stepCount - 1;
		}

		if (_isRunningSlowly)
		{
			if (_frameLag == 0) _isRunningSlowly = false;
		}
		else if (_frameLag >= 0)
		{
			_isRunningSlowly = true;
		}
	} else {
		_frameTime = _accumulatedFrameTime;
		_accumulatedFrameTime = millisecondsf(0.0f);
		Update();
	}

	Draw();
	_framesCounter++;
}

void Program::HandleEvents()
{
	char x[50];
	sprintf(x, "FPS: %d, Frame Lag: %d", _framesPerSecond, _frameLag);
	SDL_SetWindowTitle(_window, x);

	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT)
		{
			_isExiting = true;
		}
	}
}

void Program::Update()
{
	_framesPerSecondTimer += _frameTime;
	if (_framesPerSecondTimer > _oneSecondDuration)
	{
		_framesPerSecondTimer -= _oneSecondDuration;
		_framesPerSecond = _framesCounter;
		_framesCounter = 0;

		char x[10];
		sprintf(x, "FPS: %d", _framesPerSecond);
		SDL_SetWindowTitle(_window, x);
	}

	HandleEvents();

	if (_isRunningSlowly)
	{
		return;
	}

	UpdateTextures();
}

void Program::UpdateTextures()
{
	std::shared_ptr<IO::Image> img = _screenCapture.GetScreenFrameBuffer();
	_transcoder->FeedFrame(img);
	try
	{
		auto pk = _transcoder->NextPacket();
		_transcoder->FeedPacket(pk.get());

		try
		{
			auto imgDec = _transcoder->NextImage();
			_texture->UploadData(&imgDec->GetRGBBuffer()->at(0));
		}
		catch (Codec::DecoderException de)
		{

		}
	}
	catch (Codec::EncoderException ee)
	{

	}
}

void Program::Draw()
{
	glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	_texture->BindTexture();

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);

	SDL_GL_SwapWindow(_window);
}
