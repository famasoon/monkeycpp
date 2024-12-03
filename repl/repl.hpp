#pragma once
#include "../evaluator/evaluator.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
#include <iostream>

namespace monkey
{

class REPL
{
  public:
    static void Start(std::istream &in = std::cin, std::ostream &out = std::cout);

  private:
    static const std::string PROMPT;
    static void printParserErrors(std::ostream &out, const std::vector<std::string> &errors);
    static ObjectPtr evaluateInput(const std::string &input, Evaluator &evaluator);
};

} // namespace monkey
