#include <iostream>
#include <string>
#include "repl/repl.hpp"

int main()
{
  std::cout << "Hello! This is the Monkey programming language!\n";
  std::cout << "Feel free to type in commands\n";

  Repl::Start();

  return 0;
}
