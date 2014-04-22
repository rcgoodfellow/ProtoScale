#ifndef _PS_COMMANDAST_
#define _PS_COMMANDAST_

#include "FileUtil.hpp"

#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>
#include <fstream>


namespace ps { namespace cmd {

using Arg = std::string;
using Args = std::vector<std::string>;

class CommandBase
{
  public:
    enum class Kind
    {
      unknown,
      astBuild,
      pkgBuild
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

using FileName = std::string;
using FileSet = std::vector<FileName>;

class BuildASTCommand : public Command<FileSet>
{
  public:
    BuildASTCommand(Args *args) : Command{args, Kind::astBuild} {}
    FileSet operator()() const;
};

class BuildPKGCommand : public Command<FileSet>
{
  public:
    BuildPKGCommand(Args *args) : Command{args, Kind::pkgBuild} {}
    FileSet operator()() const;
};

}} //ps:cmd

#endif
