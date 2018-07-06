#pragma once
#include "Argument.h"
#include <vector>

namespace IO {
  class ArgumentNetworkAddr: public Argument {
  public:
    ArgumentNetworkAddr(std::vector<std::string> cliTex);
    CLI_ARG GetArgumentType() override;

    std::string GetIP();
    uint16_t GetPortNumber();

  private:
    std::string _ipAddr;
    uint16_t    _portNumber;
  };
}
