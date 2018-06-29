#pragma once

#include <chrono>
#include <thread>
#include <memory>

using hclock = std::chrono::high_resolution_clock;
using timepoint = std::chrono::high_resolution_clock::time_point;
using millisecondsf = std::chrono::duration<float, std::milli>;

#include "Graphics/GraphicsDebug.h"
#include "Graphics/Texture.h"
#include "Graphics/Shaders/Shader.h"
#include "Graphics/Shaders/ShaderProgram.h"
#include "Debug.h"
#include "IO/ScreenCapture.h"
#include "Codec/Transcoders/DE265Transcoder.h"
#include "Codec/Transcoders/Open264Transcoder.h"

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
	explicit Program();
#endif
	~Program();
	void Loop();

private:
	bool _isExiting;
	SDL_Window* _window;
	SDL_GLContext _context;
	timepoint _previousFrameTimePoint;
	millisecondsf _maximumElapsedTime = millisecondsf(1000.0f);
	millisecondsf _targetElapsedTime = millisecondsf(1000.0f / 24.0f);
	std::chrono::milliseconds _oneSecondDuration = std::chrono::milliseconds(1000);
	bool _isFixedUpdate = true;
	uint32_t _framesCounter = 0;
	uint32_t _framesPerSecond = 0;
	uint32_t _updateFrameLag = 0;
	bool _isRunningSlowly = false;
	Texture* _texture;
	ShaderProgram* _shaderProgram;
	GLuint _vertexArrayHandle;
	GLuint _vertexBufferHandle;
	GLuint _indexBufferHandle;
	uint32_t _windowWidth;
	uint32_t _windowHeight;
	ScreenCapture _screenCapture;
	std::shared_ptr<Codec::Transcoder> _transcoder;

	void InitializeSDL();
	void InitializeOpenGL();
	void Update();
	void Draw();
	void HandleEvents();
	void UpdateTextures();

};
