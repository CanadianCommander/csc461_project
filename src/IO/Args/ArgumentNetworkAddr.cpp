#include "ArgumentNetworkAddr.h"
#include <regex>
#include <sstream>

namespace IO {

  ArgumentNetworkAddr::ArgumentNetworkAddr(std::vector<std::string> cliTex){
    std::regex isIp ("\\d+\\.\\d+\\.\\d+\\.\\d+");

    if(cliTex.size() == 3){
      if(std::regex_match(cliTex[1], isIp)){
        _ipAddr = cliTex[1];
        std::istringstream iss (cliTex[2]);
        try{
          iss >> _portNumber;
        }
        catch(std::istringstream::failure f){
          throw ArgumentException("port number could not be decoded");
        }
      }
      else{
        throw ArgumentException("argument 2 of Network Addr does not appear to be ip address");
      }
    }
    else{
      throw ArgumentException("Wrong number of cli args for Network Addr");
    }
  }

  IO::CLI_ARG ArgumentNetworkAddr::GetArgumentType(){
    return CLI_ARG::NETWORK_ADDR;
  }

  std::string ArgumentNetworkAddr::GetIP(){
    return _ipAddr;
  }

  uint16_t ArgumentNetworkAddr::GetPortNumber(){
    return _portNumber;
  }

}
