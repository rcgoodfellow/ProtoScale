#include "Driver.hpp"
//extern Decls *mm;
//extern int cmdyyparse();
//extern int metayyparse();
//extern FILE *cmdyyin;
extern Commands *cm;

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
  std::cout << "executing command: '" << cmd << "'" << std::endl;
#endif
  cmdyy_scan_string(cmd.c_str());
  cmdyyparse();
  if(cm == NULL) 
  {
    std::cerr << "Failed to parse commands" << std::endl;
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
