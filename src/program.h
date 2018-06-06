#pragma once

#include "graphics/_graphics.h"
#include "debug.h"

class Program
{

public:
#ifndef NDEBUG
	explicit Program(LogLevel logLevel = LogLevel::TRACE, LogChannel logChannel = LogChannel::ALL);
#else
	Program();
#endif
	~Program();
	void Loop();

private:
	bool _isExiting;
	SDL_Window* _window;
	SDL_GLContext _context;
	Texture2D* _texture;
	GLuint _vertexArrayHandle;
	GLuint _vertexBufferHandle;
	GLuint _indexBufferHandle;
	uint32_t _windowWidth;
	uint32_t _windowHeight;

	void InitializeSDL();
	void InitializeOpenGL();
	void Frame();
	void HandleEvents();
	void Draw();

};