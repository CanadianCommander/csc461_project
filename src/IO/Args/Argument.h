#pragma once
#include <string>
#include <memory>

namespace IO {

  enum class CLI_ARG {
    NETWORK_MODE,
    NETWORK_ADDR
  };

  class Argument {
  public:
    virtual CLI_ARG GetArgumentType() = 0;
  };

  class ArgumentException : public std::runtime_error
  {
  public:
    ArgumentException(std::string msg) : std::runtime_error(msg)
    {};
  };

}
