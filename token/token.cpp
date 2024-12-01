#include "token.hpp"

namespace Token
{

  std::string toString(TokenType type)
  {
    switch (type)
    {
    case TokenType::ILLEGAL:
      return "ILLEGAL";
    case TokenType::EOF_:
      return "EOF";
    case TokenType::IDENT:
      return "IDENT";
    case TokenType::INT:
      return "INT";
    case TokenType::ASSIGN:
      return "=";
    case TokenType::PLUS:
      return "+";
    case TokenType::MINUS:
      return "-";
    case TokenType::BANG:
      return "!";
    case TokenType::ASTERISK:
      return "*";
    case TokenType::SLASH:
      return "/";
    case TokenType::LT:
      return "<";
    case TokenType::GT:
      return ">";
    case TokenType::EQ:
      return "==";
    case TokenType::NOT_EQ:
      return "!=";
    case TokenType::COMMA:
      return ",";
    case TokenType::SEMICOLON:
      return ";";
    case TokenType::LPAREN:
      return "(";
    case TokenType::RPAREN:
      return ")";
    case TokenType::LBRACE:
      return "{";
    case TokenType::RBRACE:
      return "}";
    case TokenType::FUNCTION:
      return "FUNCTION";
    case TokenType::LET:
      return "LET";
    case TokenType::TRUE:
      return "TRUE";
    case TokenType::FALSE:
      return "FALSE";
    case TokenType::IF:
      return "IF";
    case TokenType::ELSE:
      return "ELSE";
    case TokenType::RETURN:
      return "RETURN";
    default:
      return "UNKNOWN";
    }
  }

  // 静的メンバ変数の定義
  const std::string Token::ILLEGAL = "ILLEGAL";
  const std::string Token::EOF_ = "EOF";
  const std::string Token::IDENT = "IDENT";
  const std::string Token::INT = "INT";
  const std::string Token::ASSIGN = "=";
  const std::string Token::PLUS = "+";
  const std::string Token::MINUS = "-";
  const std::string Token::BANG = "!";
  const std::string Token::ASTERISK = "*";
  const std::string Token::SLASH = "/";
  const std::string Token::LT = "<";
  const std::string Token::GT = ">";
  const std::string Token::EQ = "==";
  const std::string Token::NOT_EQ = "!=";
  const std::string Token::COMMA = ",";
  const std::string Token::SEMICOLON = ";";
  const std::string Token::LPAREN = "(";
  const std::string Token::RPAREN = ")";
  const std::string Token::LBRACE = "{";
  const std::string Token::RBRACE = "}";
  const std::string Token::FUNCTION = "FUNCTION";
  const std::string Token::LET = "LET";
  const std::string Token::TRUE = "TRUE";
  const std::string Token::FALSE = "FALSE";
  const std::string Token::IF = "IF";
  const std::string Token::ELSE = "ELSE";
  const std::string Token::RETURN = "RETURN";

} // namespace Token