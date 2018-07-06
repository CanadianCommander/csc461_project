#pragma once
#include "Argument.h"
#include <vector>

namespace IO {
  class ArgumentNetworkMode: public Argument {
  public:
    ArgumentNetworkMode(std::vector<std::string> cliTex);
    virtual CLI_ARG GetArgumentType() override;
    bool isServer();

  private:
    bool _isServer;
  };
}
