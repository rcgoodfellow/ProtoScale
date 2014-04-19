#ifndef _PS_COMMANDAST_
#define _PS_COMMANDAST_

#include "FileUtil.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>

using Arg = std::string;
using Args = std::vector<std::string>;

class CommandBase
{
  public:
    enum class Kind
    {
      unknown,
      astBuild,
    };
    using ResultType = void;
    CommandBase(Args *args, Kind k) : args{args}, _kind{k} {}
    Kind kind() { return _kind; }


  protected:
    const Args *args;

  private:
    Kind _kind;
};
using Commands = std::vector<CommandBase*>;

template <class Result>
class Command : public CommandBase
{
  public:
    using ResultType = Result;
    using CommandBase::CommandBase;
    virtual Result operator()() const = 0;
};

struct File { std::string name; std::string content; };
using FileSet = std::vector<File>;

class BuildASTCommand : public Command<FileSet>
{
  public:
    BuildASTCommand(Args *args) : Command{args, Kind::astBuild} {}
    FileSet operator()() const;
};

#endif
