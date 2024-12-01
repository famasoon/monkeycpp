#pragma once
#include "../token/token.hpp"
#include <string>
namespace Lexer
{
  class Lexer
  {
  private:
    std::string input;
    size_t position;     // 現在の位置
    size_t readPosition; // 次の文字を読む位置
    char ch;             // 現在検査中の文字

    void readChar();
    char peekChar() const;
    std::string readIdentifier();
    std::string readNumber();
    void skipWhitespace();

  public:
    explicit Lexer(const std::string &input);
    Token::Token NextToken();
  };

} // namespace Lexer
