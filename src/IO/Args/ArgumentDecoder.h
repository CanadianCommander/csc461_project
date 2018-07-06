#pragma once
#include <vector>
#include <map>
#include <memory>
#include <string>
#include <functional>
#include "Argument.h"

namespace IO {
  using ArgInitFunction_t =  std::function<int (int, std::vector<std::string>, std::shared_ptr<Argument> *)>;

  class ArgumentDecoder{
  public:
    ArgumentDecoder();
    void ProcessCLIArguments(int argc, char ** argv);
    std::shared_ptr<Argument> GetArgByType(CLI_ARG typ);

  private:
    void _BuildArgMap(); // <-- when creating new arguments dont forget to add a mapping for it here
    std::map<std::string, ArgInitFunction_t> _argMappings;
    std::vector<std::shared_ptr<Argument>> _cliArgs;
  };

  class ArgumentDecoderException : public std::runtime_error
  {
  public:
    ArgumentDecoderException(std::string msg) : std::runtime_error(msg)
    {};
  };
}
