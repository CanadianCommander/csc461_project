#include "Program.h"


int main(int argc, char* args[])
{

#ifndef NDEBUG
	//TODO: Use command line arguments to pass log level and log channels for debug builds.
#endif
	auto isSender = false;
	if (argc > 1)
	{
		isSender = (bool)args[1];
	}

	auto program = new Program(isSender);
	program->Loop();
	program->~Program();
	return EXIT_SUCCESS;
}
