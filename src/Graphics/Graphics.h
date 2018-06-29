#pragma once

#include <SDL.h>
#include <SDL_opengl.h>

#ifdef __APPLE__
	#include <OpenGl/gl3.h>
#elif __linux__
	#define GL_GLEXT_PROTOTYPES
	#include <GL/gl.h>
#endif
