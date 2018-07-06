#include "ArgumentNetworkMode.h"

namespace IO {

  ArgumentNetworkMode::ArgumentNetworkMode(std::vector<std::string> cliTex){
    if(cliTex.size() == 1){
      if(cliTex[0].compare("-s") == 0){
        _isServer = true;
      }
      else if(cliTex[0].compare("-c") == 0){
        _isServer = false;
      }
      else{
        throw ArgumentException("Network Mode unknwon. mode is not one of < -s | -c >");
      }
    }
    else {
      throw ArgumentException("wrong number of cli arguments, expecting one!");
    }
  }

  CLI_ARG ArgumentNetworkMode::GetArgumentType(){
    return CLI_ARG::NETWORK_MODE;
  };

  bool ArgumentNetworkMode::isServer(){
    return _isServer;
  }
}
