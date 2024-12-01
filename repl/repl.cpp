#include "repl.hpp"
#include <istream>
#include <lexer.hpp>

#define PROMPT ">> "

namespace Repl
{
  void Start(std::istream &in, std::ostream &out)
  {
    Lexer::Lexer lexer(PROMPT);
    for (Token::Token tok = lexer.NextToken(); tok != Token::TokenType::EOF_; tok = lexer.NextToken())
    {
      out << tok.getType() << std::endl;
    }
  }
}
