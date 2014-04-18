#include "Driver.hpp"
extern Decls *mm;
//extern int cmdyyparse();
extern int metayyparse();
//extern FILE *cmdyyin;
extern Commands *cmds;

Driver::Driver(int argc, char **argv)
{
  globCommands(argc, argv);
}

void Driver::globCommands(int argc, char **argv)
{
  for(int i=1; i<argc; ++i)
  {
    cmd += std::string(argv[i]);
  }
}

void Driver::run()
{
  cmdyy_scan_string(cmd.c_str());
  cmdyyparse();
  if(!cmds) {

  }
}


int main(int argc, char **argv)
{
  metayyparse();
  std::cout << mm << std::endl;

  //Driver driver{argc, argv};
  //driver.run();

  return 0;
}
