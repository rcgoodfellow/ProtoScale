#include "Driver.hpp"
//extern Decls *mm;
//extern int cmdyyparse();
//extern int metayyparse();
//extern FILE *cmdyyin;

using namespace ps;
using namespace ps::cmd;

extern CommandBase *cm;

Driver::Driver(int argc, char **argv)
{
  globCommands(argc, argv);
}

void Driver::globCommands(int argc, char **argv)
{
  for(int i=1; i<argc-1; ++i)
  {
    cmd += std::string(argv[i]) + " ";
  }
  cmd += std::string(argv[argc-1]);
}

void Driver::run()
{
#ifdef DEBUG
  std::cout << "received command: '" << cmd << "'" << std::endl;
#endif
  cmdyy_scan_string(cmd.c_str());
  cmdyyparse();
  if(cm == NULL) 
  {
    std::cerr << "Failed to parse command" << std::endl;
  }
  else
  {
#ifdef DEBUG
    std::cout << "executing" << std::endl;
#endif
   
    switch(cm->kind())
    {
      case CommandBase::Kind::astBuild:
      {
        auto *tcm = static_cast<BuildASTCommand*>(cm);
        try { tcm->operator()(); }
        catch (std::exception &e){
          std::cerr << e.what() << std::endl;
        }
        break;
      }

      case CommandBase::Kind::pkgBuild:
      {
        auto *tcm = static_cast<BuildPKGCommand*>(cm);
        try { tcm->operator()(); }
        catch (std::exception &e) {
          std::cerr << e.what() << std::endl;
        }
        break;
      }

      case CommandBase::Kind::unknown :
      {
#ifdef DEBUG
        std::cout << "command type unkown" << std::endl;
#endif
        break;
      }

      defualt :
      {
#ifdef DEBUG
        std::cout << "strange command type ..." << std::endl;
#endif
        break;
      }
    }

  }
}


int main(int argc, char **argv)
{
  //metayyparse();
  //std::cout << mm << std::endl;

  Driver driver{argc, argv};
  driver.run();

  return 0;
}
