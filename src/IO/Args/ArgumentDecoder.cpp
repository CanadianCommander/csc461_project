#include "ArgumentDecoder.h"
#include "ArgumentNetworkMode.h"
#include "ArgumentNetworkAddr.h"

namespace IO {

  ArgumentDecoder::ArgumentDecoder(){
    _BuildArgMap();
  }

  void ArgumentDecoder::ProcessCLIArguments(int argc, char ** argv){
    std::vector<std::string> argVec;
    for(int i =0; i < argc; i++){
      argVec.push_back(std::string(argv[i]));
    }

    int index = 1;
    while(index < argVec.size()){
      try{
        std::shared_ptr<Argument> newPtr;
        index = _argMappings.at(argVec[index])(index, argVec,&newPtr);
        _cliArgs.push_back(newPtr);
      }
      catch(std::out_of_range e){
        throw ArgumentDecoderException("I'll formated Arguments");
      }
    }

  }

  std::shared_ptr<Argument> ArgumentDecoder::GetArgByType(CLI_ARG typ){
    for(int i =0; i < _cliArgs.size();i ++){
      if(_cliArgs[i]->GetArgumentType() == typ){
        return _cliArgs[i];
      }
    }
    return nullptr;
  }

  void ArgumentDecoder::_BuildArgMap(){
    auto networkModeInit = [] (int start, std::vector<std::string> args, std::shared_ptr<Argument> * argPtr) -> int {
      std::vector<std::string> vecSlice;
      if(start + 1 < args.size()){
       vecSlice = std::vector<std::string>(args.begin() + start, args.begin() + start + 1);
      }
      else{
        vecSlice = std::vector<std::string>(args.begin() + start, args.end());
      }
      *argPtr = std::make_shared<ArgumentNetworkMode>(vecSlice);
      return start + 1;
    };

    _argMappings.insert(std::pair<std::string, ArgInitFunction_t>("-s", networkModeInit));
    _argMappings.insert(std::pair<std::string, ArgInitFunction_t>("-c", networkModeInit));
    _argMappings.insert(std::pair<std::string, ArgInitFunction_t> ("-i", [] (int start, std::vector<std::string> args, std::shared_ptr<Argument> * argPtr) -> int {
      std::vector<std::string> vecSlice;
      if(start + 3 < args.size()){
       vecSlice = std::vector<std::string>(args.begin() + start, args.begin() + start + 3);
      }
      else{
        vecSlice = std::vector<std::string>(args.begin() + start, args.end());
      }
      *argPtr = std::make_shared<ArgumentNetworkAddr>(vecSlice);
      return start + 3;
    }));
  }
}
