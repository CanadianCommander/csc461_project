#include "Program.h"


int main(int argc, char *args[])
{
	
#ifndef NDEBUG
	//TODO: Use command line arguments to pass log level and log channels for debug builds.
#endif
	//	LogVerbose(LogCategory::ALL, "TEST");
	//	LogDebug(LogCategory::ALL, "TEST");
	//	LogWarning(LogCategory::GRAPHICS, "TEST");
	//	LogInfo(LogCategory::NETWORK, "TEST");
	//	LogError(LogCategory::ALL, "TEST");
	//	LogCritical(LogCategory::ALL, "TEST");

	auto program = Program();
	program.Loop();
	program.~Program();
	return EXIT_SUCCESS;
}
