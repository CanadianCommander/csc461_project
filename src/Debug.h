#pragma once

#include <cstdarg>
#include <cstdio>
#include <string>
#include <vector>

using std::string;

#ifndef NDEBUG

enum class LogPriority
{
	VERBOSE = 0, DEBUG = 1, INFO = 2, WARNING = 3, ERROR = 4, CRITICAL = 5
};

enum class LogCategory
{
	NONE = 0,
	IO = 1 << 0,
	GRAPHICS = 1 << 1,
	NETWORK = 1 << 2,
	CODEC = 1 << 3,
	ALL = IO | GRAPHICS | NETWORK | CODEC,
};

//https://en.wikipedia.org/wiki/ANSI_escape_code
#define CSI_ATTRIBUTE(attribute) "\033[" #attribute "m"
#define CSI_ATTRIBUTE_FGCOLOR_BGCOLOR(attribute, fgColor, bgColor) "\033[" #attribute "m\033[38;5;" #fgColor "m\033[48;5;" #bgColor "m"
#define CSI_RESET "\033[;m"

#endif

#define LogVerbose(category, format, args...) Log(LogPriority::VERBOSE, category, format, ##args)
#define LogDebug(category, format, args...) Log(LogPriority::DEBUG, category, format, ##args)
#define LogInfo(category, format, args...) Log(LogPriority::INFO, category, format, ##args)
#define LogWarning(category, format, args...) Log(LogPriority::WARNING, category, format, ##args)
#define LogError(category, format, args...) Log(LogPriority::ERROR, category, format, ##args)
#define LogCritical(category, format, args...) { Log(LogPriority::CRITICAL, category, format, ##args); exit(EXIT_FAILURE); }

#ifdef NDEBUG
#define Log(level, channels, format, args...) ((void)0)
#define LogExit(priority, category, format, args...) ((void)0)
#define SetLogPriority() ((void)0)
#define SetLogCategory() ((void)0)
#define LogSDL(TEXT) ((void)0)
#define LogVerboseOrElseCritical(category, predicate, text) ((void)0)
#else
#define Log(level, category, format, args...) \
    LogReal(level, category, __FILE__, __LINE__, __PRETTY_FUNCTION__, format, ##args)
#define LogExit(priority, category, format, args...) \
    { LogReal(priority, category, __FILE__, __LINE__, __PRETTY_FUNCTION__, format, ##args); exit(EXIT_FAILURE); }
#define SetLogPriority(priority) \
    SetLogPriorityReal(priority)
#define SetLogCategory(category) \
    SetLogCategoryReal  (category)

void
LogReal(LogPriority priority, LogCategory category, const string &file, int line, const string &function, string format,
        ...);
void SetLogPriorityReal(LogPriority priority);
void SetLogCategoryReal(LogCategory category);

extern const char* g_sdlError;
#define LogSDL(TEXT)                                                                                \
{                                                                                                   \
    g_sdlError = SDL_GetError();                                                                    \
    if(g_sdlError[0] != '\0')                                                                       \
    {                                                                                               \
        LogCritical(LogCategory::ALL, #TEXT ": %s. SDL_Error: %s", toStatus(false), g_sdlError);    \
    } else                                                                                          \
    {                                                                                               \
        LogVerbose(LogCategory::ALL, #TEXT ": %s.", toStatus(true));                                \
    }                                                                                               \
}

#define LogVerboseOrElseCritical(category, predicate, text)                                         \
{                                                                                                   \
    if (predicate)                                                                                  \
    {                                                                                               \
        LogVerbose(category, #text ": %s.", toStatus(true));                                        \
    }                                                                                               \
    else                                                                                            \
    {                                                                                               \
        LogCritical(category, #text ": %s.", toStatus(false));                                      \
    }                                                                                               \
}

#endif

inline const char* const toStatus(bool boolean)
{
	return boolean ? "SUCCESS" : "FAILURE";
}
