#ifndef _PS_COMMANDAST_
#define _PS_COMMANDAST_

#include <vector>
#include <string>

using Arg = std::string;
using Args = std::vector<std::string>;

class CommandBase
{
  public:
    CommandBase(Args *args) : args{args} {}
  protected:
    const Args *args;
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

using FileSet = std::vector<std::string>;

class BuildASTCommand : public Command<FileSet>
{
  public:
    using Command<FileSet>::Command;
    FileSet operator()() const;
};

#endif
