#include "ast.hpp"

namespace AST
{
  // Program implementation
  std::string Program::TokenLiteral() const
  {
    if (!statements.empty())
    {
      return statements[0]->TokenLiteral();
    }
    return "";
  }

  std::string Program::String() const
  {
    std::string out;
    for (size_t i = 0; i < statements.size(); i++)
    {
      if (!statements[i])
        continue;

      out += statements[i]->String();
    }
    return out;
  }

  // Identifier implementation
  Identifier::Identifier(Token::Token token, std::string value)
      : token(std::move(token)), value(std::move(value)) {}

  void Identifier::expressionNode() {}

  std::string Identifier::TokenLiteral() const
  {
    return token.literal;
  }

  std::string Identifier::String() const
  {
    return value;
  }

  // LetStatement implementation
  LetStatement::LetStatement(Token::Token token)
      : token(std::move(token)) {}

  void LetStatement::statementNode() {}

  std::string LetStatement::TokenLiteral() const
  {
    return token.literal;
  }

  std::string LetStatement::String() const
  {
    std::string result = TokenLiteral() + " ";
    if (name)
    {
      result += name->String();
    }
    result += " = ";
    if (value)
    {
      result += value->String();
    }
    result += ";";
    return result;
  }

  // ReturnStatement implementation
  ReturnStatement::ReturnStatement(Token::Token token)
      : token(std::move(token)) {}

  void ReturnStatement::statementNode() {}

  std::string ReturnStatement::TokenLiteral() const
  {
    return token.literal;
  }

  std::string ReturnStatement::String() const
  {
    std::string result = TokenLiteral() + " ";
    if (returnValue)
    {
      result += returnValue->String();
    }
    result += ";";
    return result;
  }

  // ExpressionStatement implementation
  ExpressionStatement::ExpressionStatement(Token::Token token)
      : token(std::move(token)) {}

  void ExpressionStatement::statementNode() {}

  std::string ExpressionStatement::TokenLiteral() const
  {
    return token.literal;
  }

  std::string ExpressionStatement::String() const
  {
    if (!expression)
      return "";

    // セミコロンを含めない文字列表現を返す
    return expression->String();
  }
} // namespace AST
