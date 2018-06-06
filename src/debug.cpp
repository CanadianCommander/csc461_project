#include "debug.h"

string logLevels[5] = {
		"TRACE",
		"DEBUG",
		"INFO",
		"WARNING",
		"ERROR"
};
string logLevelColors[5] = {
		COLOR_CODE(7, 37),
		COLOR_CODE_EXT(7, 34, 47),
		COLOR_CODE(7, 32),
		COLOR_CODE(7, 33),
		COLOR_CODE(7, 31)
};

#ifndef NDEBUG

LogLevel g_logLevel = LogLevel::TRACE;
LogChannel g_logChannels = LogChannel::ALL;
const char* g_sdlError = nullptr;

void LogReal(LogLevel level, LogChannel channels, const string &filePath, int line, const string &function, string format, ...)
{
	va_list args;
	if ((level >= g_logLevel) && ((channels & g_logChannels) != 0))
	{
		va_start(args, format);
		fprintf(stderr, "%s%s%s [%s %i] %s%s:%s\n", logLevelColors[level].c_str(), logLevels[level].c_str(),
		        RESET_COLOR, filePath.c_str(), line, COLOR_CODE(1, 30), function.c_str(), RESET_COLOR);
		format.append("\n");
		vfprintf(stderr, format.c_str(), args);
		va_end(args);
	}
}

void SetLogLevelReal(LogLevel level)
{
	g_logLevel = level;
}

void SetLogChannelsReal(LogChannel channels)
{
	g_logChannels = channels;
}

#endif