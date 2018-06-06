#pragma once

#include <SDL.h>
#include <OpenGl/gl3.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

#include "../debug.h"

#ifdef NDEBUG
#define glCheckErrors(TEXT) ((void)0)
#define LogGL(TEXT) ((void)0)
#else
extern GLenum g_glError;
#define glCheckErrors(TEXT)                                                 \
{                                                                           \
	g_glError = GL_NO_ERROR;                                                \
    while ((g_glError = glGetError()) != GL_NO_ERROR)                       \
    {                                                                       \
        LogError(LogCategory::GRAPHICS, #TEXT " GLError: %d", g_glError);    \
    }                                                                       \
}
#define LogGL(TEXT)                                                                                 \
{                                                                                                   \
	g_glError = GL_NO_ERROR;                                                                        \
    if((g_glError = glGetError()) != GL_NO_ERROR)                                                   \
    {                                                                                               \
        LogError(LogCategory::GRAPHICS, #TEXT ": %s. GLError: %d", toStatus(false), g_glError);      \
		exit(EXIT_FAILURE);                                                                         \
    } else                                                                                          \
	{                                                                                               \
    	LogVerbose(LogCategory::GRAPHICS, #TEXT ": %s.", toStatus(true));                              \
    }                                                                                               \
}
#endif