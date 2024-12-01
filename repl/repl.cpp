#include "repl.hpp"

namespace Repl
{

  void Start(std::istream &in, std::ostream &out)
  {
    std::string line;

    while (true)
    {
      out << PROMPT;

      if (!std::getline(in, line) || line == "exit")
      {
        out << "Goodbye!\n";
        return;
      }

      if (line.empty())
      {
        continue;
      }

      Lexer::Lexer lexer(line);

      for (Token::Token tok = lexer.NextToken();
           tok.type != Token::TokenType::EOF_;
           tok = lexer.NextToken())
      {
        out << "{ Type: " << tok.getType()
            << ", Literal: " << tok.literal << " }\n";
      }
    }
  }

} // namespace Repl
