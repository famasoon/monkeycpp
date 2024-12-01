#include "repl.hpp"
#include <iostream>
#include <string>

namespace Repl {

void Start() {
  std::string line;

  while (true) {
    std::cout << PROMPT;
    
    if (!std::getline(std::cin, line)) {
      return;
    }

    Lexer::Lexer lexer(line);
    
    for (Token::Token tok = lexer.NextToken(); 
         tok.type != Token::TokenType::EOF_; 
         tok = lexer.NextToken()) {
      std::cout << "{ Type: " << tok.getType() 
                << ", Literal: " << tok.literal << " }\n";
    }
  }
}

} // namespace Repl
