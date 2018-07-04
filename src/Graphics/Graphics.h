#pragma once

#include <SDL.h>

#ifdef __APPLE__

#include <OpenGl/gl3.h>

#elif __linux__

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

#endif

#ifndef NDEBUG

#include "GraphicsDebug.h"

#endif
