#include "lexer.hpp"
#include <cctype>

namespace Lexer
{

  Lexer::Lexer(const std::string &input)
      : input(input), position(0), readPosition(0), ch(0)
  {
    readChar();
  }

  void Lexer::readChar()
  {
    if (readPosition >= input.length())
    {
      ch = 0;
    }
    else
    {
      ch = input[readPosition];
    }
    position = readPosition;
    readPosition++;
  }

  char Lexer::peekChar() const
  {
    if (readPosition >= input.length())
    {
      return 0;
    }
    return input[readPosition];
  }

  std::string Lexer::readIdentifier()
  {
    auto pos = position;
    while (std::isalpha(ch) || ch == '_')
    {
      readChar();
    }
    return input.substr(pos, position - pos);
  }

  std::string Lexer::readNumber()
  {
    auto pos = position;
    while (std::isdigit(ch))
    {
      readChar();
    }
    return input.substr(pos, position - pos);
  }

  void Lexer::skipWhitespace()
  {
    while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
    {
      readChar();
    }
  }

  Token::Token Lexer::NextToken()
  {
    skipWhitespace();

    Token::Token tok(Token::TokenType::ILLEGAL, std::string(1, ch));

    switch (ch)
    {
    case '=':
      if (peekChar() == '=')
      {
        readChar();
        tok = Token::Token(Token::TokenType::EQ, "==");
      }
      else
      {
        tok = Token::Token(Token::TokenType::ASSIGN, "=");
      }
      break;
    case '+':
      tok = Token::Token(Token::TokenType::PLUS, "+");
      break;
    case '-':
      tok = Token::Token(Token::TokenType::MINUS, "-");
      break;
    case '!':
      if (peekChar() == '=')
      {
        readChar();
        tok = Token::Token(Token::TokenType::NOT_EQ, "!=");
      }
      else
      {
        tok = Token::Token(Token::TokenType::BANG, "!");
      }
      break;
    case '*':
      tok = Token::Token(Token::TokenType::ASTERISK, "*");
      break;
    case '/':
      tok = Token::Token(Token::TokenType::SLASH, "/");
      break;
    case '<':
      tok = Token::Token(Token::TokenType::LT, "<");
      break;
    case '>':
      tok = Token::Token(Token::TokenType::GT, ">");
      break;
    case ';':
      tok = Token::Token(Token::TokenType::SEMICOLON, ";");
      break;
    case '(':
      tok = Token::Token(Token::TokenType::LPAREN, "(");
      break;
    case ')':
      tok = Token::Token(Token::TokenType::RPAREN, ")");
      break;
    case ',':
      tok = Token::Token(Token::TokenType::COMMA, ",");
      break;
    case '{':
      tok = Token::Token(Token::TokenType::LBRACE, "{");
      break;
    case '}':
      tok = Token::Token(Token::TokenType::RBRACE, "}");
      break;
    case 0:
      tok = Token::Token(Token::TokenType::EOF_, "");
      break;
    default:
      if (std::isalpha(ch) || ch == '_')
      {
        auto literal = readIdentifier();
        // キーワードの判定は別の関数に分けることもできます
        if (literal == "fn")
        {
          return Token::Token(Token::TokenType::FUNCTION, literal);
        }
        else if (literal == "let")
        {
          return Token::Token(Token::TokenType::LET, literal);
        }
        else if (literal == "true")
        {
          return Token::Token(Token::TokenType::TRUE, literal);
        }
        else if (literal == "false")
        {
          return Token::Token(Token::TokenType::FALSE, literal);
        }
        else if (literal == "if")
        {
          return Token::Token(Token::TokenType::IF, literal);
        }
        else if (literal == "else")
        {
          return Token::Token(Token::TokenType::ELSE, literal);
        }
        else if (literal == "return")
        {
          return Token::Token(Token::TokenType::RETURN, literal);
        }
        return Token::Token(Token::TokenType::IDENT, literal);
      }
      else if (std::isdigit(ch))
      {
        return Token::Token(Token::TokenType::INT, readNumber());
      }
      break;
    }

    readChar();
    return tok;
  }

} // namespace Lexer