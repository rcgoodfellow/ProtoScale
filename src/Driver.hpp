#include "MetaAST.hpp"
#include "CommandAST.hpp"
#include "CommandParser.hpp"
#include "CommandScanner.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>

class Driver
{
  public:
    Driver(int argc, char **argv);
    void run();

  private:
    void globCommands(int argc, char **argv); 
    std::string cmd{""};
};
