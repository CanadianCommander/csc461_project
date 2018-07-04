#pragma once

#include <chrono>
#include <thread>
#include <memory>

using hclock = std::chrono::high_resolution_clock;
using timepoint = std::chrono::high_resolution_clock::time_point;
using millisecondsf = std::chrono::duration<float, std::milli>;

#include "Graphics/Graphics.h"
#include "Graphics/Texture.h"
#include "Debug.h"
#include "IO/ScreenCapture.h"
#include "Codec/Transcoders/DE265Transcoder.h"

using IO::ScreenCapture;

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
	timepoint _previousFrameTimePoint;
	millisecondsf _frameTime = millisecondsf(0);
	millisecondsf _framesPerSecondTimer = millisecondsf(0);
	millisecondsf _maximumElapsedTime = millisecondsf(2000.0f);
	millisecondsf _targetElapsedTime = millisecondsf(1000.0f / 20.0f);
	millisecondsf _accumulatedFrameTime = _targetElapsedTime;
	std::chrono::milliseconds _oneSecondDuration = std::chrono::milliseconds(1000);
	bool _isFixedUpdate = true;
	uint32_t _framesCounter = 0;
	uint32_t _framesPerSecond = 0;
	uint32_t _frameLag = 0;
	bool _isRunningSlowly = false;
	Graphics::Texture* _texture;
	GLuint _vertexArrayHandle;
	GLuint _vertexBufferHandle;
	GLuint _indexBufferHandle;
	uint32_t _windowWidth;
	uint32_t _windowHeight;
	ScreenCapture _screenCapture;
	std::shared_ptr<Codec::Transcoder> _transcoder;

	void InitializeSDL();
	void InitializeOpenGL();
	void Frame();
	void Update();
	void Draw();
	void HandleEvents();
	void UpdateTextures();

};
