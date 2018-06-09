#pragma once

#include <SDL.h>
#include <OpenGl/gl3.h>

#include "../Debug.h"

#ifdef NDEBUG
#define LogsGL(text) ((void)0)
#define LogGL(text) ((void)0)
#define LogGLWithHandle(text, handle) ((void)0)
#else
extern GLenum g_glError;
#define LogsGL(text)                                                        \
{                                                                           \
	g_glError = GL_NO_ERROR;                                                \
    while ((g_glError = glGetError()) != GL_NO_ERROR)                       \
    {                                                                       \
        LogCritical(LogCategory::GRAPHICS, #text " GLError: %d.", g_glError);\
    }                                                                       \
}
#define LogGL(text)                                                                                 \
{                                                                                                   \
	g_glError = GL_NO_ERROR;                                                                        \
    if((g_glError = glGetError()) != GL_NO_ERROR)                                                   \
    {                                                                                               \
        LogCritical(LogCategory::GRAPHICS, #text ": %s. GLError: %d.", toStatus(false), g_glError); \
    } else                                                                                          \
	{                                                                                               \
    	LogVerbose(LogCategory::GRAPHICS, #text ": %s.", toStatus(true));                           \
    }                                                                                               \
}
#define LogGLWithHandle(text, handle)                                                               \
{                                                                                                   \
	g_glError = GL_NO_ERROR;                                                                        \
    if((g_glError = glGetError()) != GL_NO_ERROR)                                                   \
    {                                                                                               \
        LogCritical(LogCategory::GRAPHICS, #text ": %s. GLError: %d.", toStatus(false), g_glError); \
    } else                                                                                          \
	{                                                                                               \
    	LogVerbose(LogCategory::GRAPHICS, #text ": %s. Handle: %d.", toStatus(true), handle);       \
    }                                                                                               \
}
#endif