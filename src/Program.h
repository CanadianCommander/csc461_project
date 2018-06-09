#pragma once

#include <chrono>
#include <thread>

using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::high_resolution_clock::time_point;
using TimeSpan = std::chrono::duration<float, std::milli>;

#include "Graphics/GraphicsDebug.h"
#include "Graphics/Texture.h"
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ShaderProgram.h"
#include "Debug.h"
#include "IO/ScreenCapture.h"

using Graphics::Texture;
using Graphics::Shaders::Shader;
using Graphics::Shaders::ShaderProgram;
using IO::ScreenCapture;
using IO::Image;


class Program
{

public:
#ifndef NDEBUG
	explicit Program(LogPriority logPriority = LogPriority::VERBOSE, LogCategory logCategory = LogCategory::ALL);
#else
	Program();
#endif
	~Program();
	void Loop();

private:
	bool _isExiting;
	SDL_Window* _window;
	SDL_GLContext _context;
	TimePoint _previousFrameTimePoint;
	TimeSpan _oneSecondDuration = TimeSpan(1000.0f);
	TimeSpan _stepTime = TimeSpan(1000.0f / 24.0f);
	TimeSpan _deltaTime;
	TimeSpan _framePerSecondTime;
	uint32_t _framesCounter = 0;
	uint32_t _framesPerSecond = 0;
	Texture* _texture;
	ShaderProgram* _shaderProgram;
	GLuint _vertexArrayHandle;
	GLuint _vertexBufferHandle;
	GLuint _indexBufferHandle;
	uint32_t _windowWidth;
	uint32_t _windowHeight;
	ScreenCapture _screenCapture;

	void InitializeSDL();
	void InitializeOpenGL();
	void Frame();
	void HandleEvents();
  void UpdateTextures();
	void Draw();

};
