#include "Debug.h"

string logPriorityStrings[6] = {
		"VERBOSE",
		"DEBUG",
		"INFO",
		"WARNING",
		"ERROR",
		"CRITICAL"
};
string logPriorityFormats[6] = {
		CSI_ATTRIBUTE_FGCOLOR_BGCOLOR(0, 231, 232),
		CSI_ATTRIBUTE_FGCOLOR_BGCOLOR(0, 231, 28),
		CSI_ATTRIBUTE_FGCOLOR_BGCOLOR(0, 232, 27),
		CSI_ATTRIBUTE_FGCOLOR_BGCOLOR(5, 232, 226),
		CSI_ATTRIBUTE_FGCOLOR_BGCOLOR(5, 232, 196),
		CSI_ATTRIBUTE_FGCOLOR_BGCOLOR(5, 196, 232)
};

string logCategoryStrings[4] = {
		"All",
		"Graphics",
		"Network",
		"Codec"
};

string logCategoryFormats[4] = {
		CSI_ATTRIBUTE_FGCOLOR_BGCOLOR(0, 232, 231),
		CSI_ATTRIBUTE_FGCOLOR_BGCOLOR(0, 231, 73),
		CSI_ATTRIBUTE_FGCOLOR_BGCOLOR(0, 232, 55),
		CSI_ATTRIBUTE_FGCOLOR_BGCOLOR(0, 0, 40),
};

#ifndef NDEBUG

LogPriority g_logPriority = LogPriority::VERBOSE;
LogCategory g_logCategory = LogCategory::ALL;
const char* g_sdlError = nullptr;
std::vector<char> g_buffer();

#define PRINT_CATEGORY_BY_INDEX(index) fprintf(stderr, "%s %3.8s %s", logCategoryFormats[(index)].c_str(), logCategoryStrings[(index)].c_str(), CSI_RESET);

void LogReal(LogPriority priority, uint32_t category, const string &filePath, int line, const string &function,
             string format, ...)
{
	va_list args;
	if ((priority >= g_logPriority) && ((category & g_logCategory) != 0))
	{
		va_start(args, format);
		fprintf(stderr, "%s%9s %s", logPriorityFormats[priority].c_str(),
		        logPriorityStrings[priority].c_str(),
		        CSI_RESET);
		if (category == LogCategory::ALL)
		{
			PRINT_CATEGORY_BY_INDEX(0);
		}
		else
		{
			if ((category & LogCategory::GRAPHICS) != 0) PRINT_CATEGORY_BY_INDEX(1);
			if ((category & LogCategory::NETWORK) != 0) PRINT_CATEGORY_BY_INDEX(2);
			if ((category & LogCategory::CODEC) != 0) PRINT_CATEGORY_BY_INDEX(3);
		}

		fprintf(stderr, " [%s, %i] %s%s;%s", filePath.c_str(), line, CSI_ATTRIBUTE_FGCOLOR_BGCOLOR(1, 231, 232),
		        function.c_str(),
		        CSI_RESET);
		fprintf(stderr, "\n");
		format.append("\n");
		vfprintf(stderr, format.c_str(), args);
		va_end(args);
	}
}

void SetLogPriorityReal(LogPriority priority)
{
	g_logPriority = priority;
}

void SetLogCategoryReal(LogCategory category)
{
	g_logCategory = category;
}

#endif