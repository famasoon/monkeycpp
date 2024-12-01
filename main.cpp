#include <iostream>
#include "repl/repl.hpp"

int main()
{
  std::cout << "Hello! This is the Monkey programming language!" << std::endl;
  std::cout << "Feel free to type in commands" << std::endl;
  
  Repl::Start(std::cin, std::cout);
  
  return 0;
}
