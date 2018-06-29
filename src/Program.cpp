#include "Debug.h"
#include "Program.h"

#ifndef NDEBUG

Program::Program(LogPriority logPriority, LogCategory logCategory)
{
	SetLogPriority(logPriority);
	SetLogCategory(logCategory);
#else
	Program::Program()
	{
#endif
	InitializeSDL();
	InitializeOpenGL();
	_isExiting = false;
	_transcoder = std::make_shared<Codec::Open264Transcoder>();
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

	_shaderProgram = new ShaderProgram(vertexShaderSource, fragmentShaderSource);

	auto vertexPositionAttributeLocation = _shaderProgram->Attributes()["vertexPosition"];
	glEnableVertexAttribArray(vertexPositionAttributeLocation);
	LogGL("glEnableVertexAttribArray");
	glVertexAttribPointer(vertexPositionAttributeLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
	LogGL("glVertexAttribPointer");

	auto vertexTextureCoordinateLocation = _shaderProgram->Attributes()["vertexTextureCoordinate"];
	LogGL("glGetAttribLocation");
	glEnableVertexAttribArray(vertexTextureCoordinateLocation);
	LogGL("glEnableVertexAttribArray");
	glVertexAttribPointer(vertexTextureCoordinateLocation, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
	                      (void*)(2 * sizeof(GLfloat)));
	LogGL("glVertexAttribPointer");

	glActiveTexture(GL_TEXTURE0);
	LogGL("glActiveTexture");

	auto image = _screenCapture.GetScreenFrameBuffer();

	_texture = new Texture(image->GetWidth(), image->GetHeight());
	_texture->UploadImage(image.get());

	auto textureSamplerUniformLocation = _shaderProgram->Uniforms()["textureSampler"];
	glUniform1i(textureSamplerUniformLocation, 0);
	LogGL("glUniform1i");
}

void Program::Loop()
{

	auto previousUpdateTimePoint = hclock::now();
	auto accumulatedUpdateTime = millisecondsf(0.0f);
	auto accumulatedDrawTime = millisecondsf(0.0f);
	auto updateTime = millisecondsf(0.0f);

	while (!_isExiting)
	{
		RetryTick:
		auto currentUpdateTimePoint = hclock::now();
		accumulatedUpdateTime = std::chrono::duration_cast<millisecondsf>(currentUpdateTimePoint - previousUpdateTimePoint);
		previousUpdateTimePoint = currentUpdateTimePoint;

		if (_isFixedUpdate && accumulatedUpdateTime < _targetElapsedTime)
		{
			auto sleepDuration = _targetElapsedTime - accumulatedUpdateTime;
			std::this_thread::sleep_for(sleepDuration);
			goto RetryTick;
		}

		if (accumulatedUpdateTime > _maximumElapsedTime) accumulatedUpdateTime = _maximumElapsedTime;

		if (_isFixedUpdate)
		{
			uint8_t stepCount = 0;
			while (accumulatedUpdateTime >= _targetElapsedTime)
			{
				accumulatedUpdateTime -= _targetElapsedTime;
				stepCount++;
				Update();
			}

			if (stepCount == 1)
			{
				if (_updateFrameLag > 0) _updateFrameLag--;
			} else {
				_updateFrameLag += stepCount - 1;
			}

			if (_isRunningSlowly)
			{
				if (_updateFrameLag == 0) _isRunningSlowly = false;
			}
			else if (_updateFrameLag >= 5)
			{
				_isRunningSlowly = true;
			}

			updateTime = std::chrono::duration_cast<std::chrono::milliseconds>(_targetElapsedTime) * stepCount;
		} else {
			updateTime = accumulatedUpdateTime;
			accumulatedUpdateTime = millisecondsf(0.0f);
			Update();
		}

		_framesCounter++;
		Draw();

		accumulatedDrawTime += updateTime;
		if (accumulatedDrawTime > _oneSecondDuration)
		{
			accumulatedDrawTime -= _oneSecondDuration;
			_framesPerSecond = _framesCounter;
			_framesCounter = 0;

			char x[10];
			sprintf(x, "FPS: %d", _framesPerSecond);
			SDL_SetWindowTitle(_window, x);
		}
	}
}

void Program::Update()
{
	UpdateTextures();
	HandleEvents();
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
	}
}

void Program::UpdateTextures()
{
//	auto image = _screenCapture.GetScreenFrameBuffer();
//
//	_transcoder->FeedFrame(image);
//	try
//	{
//		auto packet = _transcoder->NextPacket();
//		_transcoder->FeedPacket(packet.get());
//
//		try
//		{
//			auto imageDecoded = _transcoder->NextImage();
//			_texture->UploadImage(imageDecoded.get());
//		}
//		catch (const Codec::DecoderException &de)
//		{
//			LogCritical(LogCategory::CODEC, "hmmm");
//		}
//	}
//	catch (const Codec::EncoderException &ee)
//	{
//		LogCritical(LogCategory::CODEC, "hmmm");
//	}
}

void Program::Draw()
{
	glClearColor(0, 0, 0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);

	SDL_GL_SwapWindow(_window);
}
