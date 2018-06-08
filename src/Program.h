#pragma once

#include "Graphics/Graphics.h"
#include "Graphics/Texture.h"
#include "Debug.h"
#include "IO/ScreenCapture.h"

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
	Graphics::Texture* _texture;
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