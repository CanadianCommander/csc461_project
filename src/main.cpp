#include "Program.h"
#include "IO/Args/ArgumentDecoder.h"
#include "IO/Args/ArgumentNetworkMode.h"
#include "IO/Args/ArgumentNetworkAddr.h"
#include <iostream>

int main(int argc, char ** argv)
{

#ifndef NDEBUG
	//TODO: Use command line arguments to pass log level and log channels for debug builds.
#endif
	//parse cli arguments
	IO::ArgumentDecoder argDecoder;
	argDecoder.ProcessCLIArguments(argc, argv);
	std::shared_ptr<IO::ArgumentNetworkMode> mode = std::dynamic_pointer_cast<IO::ArgumentNetworkMode>(argDecoder.GetArgByType(IO::CLI_ARG::NETWORK_MODE));
	std::shared_ptr<IO::ArgumentNetworkAddr> addr = std::dynamic_pointer_cast<IO::ArgumentNetworkAddr>(argDecoder.GetArgByType(IO::CLI_ARG::NETWORK_ADDR));


	if(addr || mode){
		auto program = new Program(mode->isServer(), addr->GetIP(), addr->GetPortNumber());
		program->Loop();
		program->~Program();
		return EXIT_SUCCESS;
	}
	else {
		std::cout << "One or more missing arguments. format is. (-s | -c) -i <ip addr> <portNum> \n" <<
		 							"-s means server\n-c means client\n ex: -s -i 127.0.0.1 8080 means create a server and send frame data to ip address 127.0.0.1 @ port 8080\n";
		return EXIT_FAILURE;
	}
}
