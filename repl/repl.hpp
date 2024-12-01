#pragma once
#include "../lexer/lexer.hpp"
#include <string>
#include <iostream>

namespace Repl
{
  const std::string PROMPT = ">> ";
  void Start(std::istream &in = std::cin, std::ostream &out = std::cout);
}
