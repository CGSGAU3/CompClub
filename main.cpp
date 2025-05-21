#include <iostream>

#include "club_manager.h"

int main( int argC, char *argV[] )
{
  std::string fileName;

  if (argC == 1) // for test only, need to be removed
  {
    std::cout << "Input filename: ";
    std::getline(std::cin, fileName);
  }
  else if (argC == 2)
  {
    fileName = argV[1];
  }
  else
  {
    std::cerr << "Incorrect usage!" << std::endl;
    std::cerr << "Usage: cmpclb.exe <input file>" << std::endl;
    return EXIT_FAILURE;
  }

  try
  {
    ClubManager(fileName).work();
  }
  catch ( const std::exception &e )
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }
  catch ( const std::string &str )
  {
    std::cerr << str << std::endl;
    return EXIT_FAILURE;
  }
}