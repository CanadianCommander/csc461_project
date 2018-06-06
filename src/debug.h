#pragma once

#include <cstdarg>
#include <cstdio>
#include <string>

using std::string;

#ifndef NDEBUG

enum LogLevel
{
	TRACE = 0, DEBUG = 1, INFO = 2, WARNING = 3, ERROR = 4
};

enum LogChannel
{
	GRAPHICS = 1u << 0,
	ALL = GRAPHICS,
};

#endif

#define COLOR_CODE(attribute, color) "\033[" #attribute ";" #color "m"
#define RESET_COLOR COLOR_CODE(0, 0)
#define COLOR_CODE_EXT(attribute, fgColor, bgColor) \
  "\033[" #attribute ";" #fgColor "m\033[" #bgColor "m"

#define LogTrace(channels, format, args...) Log(TRACE, channels, format, ##args)
#define LogDebug(channels, format, args...) Log(DEBUG, channels, format, ##args)
#define LogInfo(channels, format, args...) Log(INFO, channels, format, ##args)
#define LogWarning(channels, format, args...) Log(WARNING, channels, format, ##args)
#define LogError(channels, format, args...) Log(ERROR, channels, format, ##args)

#ifdef NDEBUG
#define Log(level, channels, format, args...) ((void)0)
#define SetLogLevel() ((void)0)
#define SetLogChannels() ((void)0)
#else
#define Log(level, channels, format, args...) \
	LogReal(level, channels, __FILE__, __LINE__, __PRETTY_FUNCTION__, format, ##args)
#define SetLogLevel(level) \
	SetLogLevelReal(level)
#define SetLogChannels(channels) \
	SetLogChannelsReal(channels)

void LogReal(LogLevel level, LogChannel channels, const string& file, int line, const string& function, string format, ...);
void SetLogLevelReal(LogLevel level);
void SetLogChannelsReal(LogChannel channels);

extern const char* g_sdlError;
#define LogSDL(TEXT)                                                                                \
{                                                                                                   \
	g_sdlError = SDL_GetError();                                                                    \
    if(g_sdlError[0] != '\0')                                                                       \
    {                                                                                               \
        LogError(LogChannel::ALL, #TEXT ": %s. SDL_Error: %s", toStatus(false), g_sdlError);        \
		exit(EXIT_FAILURE);                                                                         \
    } else                                                                                          \
	{                                                                                               \
    	LogTrace(LogChannel::ALL, #TEXT ": %s.", toStatus(true));                                   \
    }                                                                                               \
}

#endif

inline const char *const toStatus(bool boolean)
{
	return boolean ? "SUCCESS" : "FAILURE";
}