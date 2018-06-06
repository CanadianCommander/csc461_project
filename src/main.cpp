#include "program.h"

int main(int argc, char *args[])
{
#ifndef NDEBUG
	//TODO: Use command line arguments to pass log level and log channels for debug builds.
#endif
	auto program = Program();
	program.Loop();
	program.~Program();
	return EXIT_SUCCESS;
}
