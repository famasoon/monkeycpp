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
      return "ASSIGN";
    case TokenType::PLUS:
      return "PLUS";
    case TokenType::MINUS:
      return "MINUS";
    case TokenType::BANG:
      return "BANG";
    case TokenType::ASTERISK:
      return "ASTERISK";
    case TokenType::SLASH:
      return "SLASH";
    case TokenType::LT:
      return "LT";
    case TokenType::GT:
      return "GT";
    case TokenType::EQ:
      return "EQ";
    case TokenType::NOT_EQ:
      return "NOT_EQ";
    case TokenType::COMMA:
      return "COMMA";
    case TokenType::SEMICOLON:
      return "SEMICOLON";
    case TokenType::LPAREN:
      return "LPAREN";
    case TokenType::RPAREN:
      return "RPAREN";
    case TokenType::LBRACE:
      return "LBRACE";
    case TokenType::RBRACE:
      return "RBRACE";
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
    case TokenType::STRING:
      return "STRING";
    case TokenType::LBRACKET:
      return "LBRACKET";
    case TokenType::RBRACKET:
      return "RBRACKET";
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
  const std::string Token::LBRACKET = "[";
  const std::string Token::RBRACKET = "]";

  Token::Token(TokenType type, std::string literal)
      : type(type), literal(std::move(literal)) {}

  bool Token::operator==(const Token &other) const {
      return type == other.type && literal == other.literal;
  }

  bool Token::operator==(const TokenType &other) const {
      return type == other;
  }

  bool Token::operator!=(const Token &other) const {
      return !(*this == other);
  }

  bool Token::operator!=(const TokenType &other) const {
      return type != other;
  }

  std::string Token::getType() const {
      return toString(type);
  }

} // namespace Token