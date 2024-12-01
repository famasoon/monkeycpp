#include "ast.hpp"

namespace AST
{
  // Program implementation
  std::string Program::TokenLiteral() const
  {
    if (!statements.empty() && statements[0])
    {
      return statements[0]->TokenLiteral();
    }
    return "";
  }

  std::string Program::String() const
  {
    std::string out;
    for (const auto& stmt : statements)
    {
      if (stmt)
      {
        out += stmt->String();
      }
    }
    return out;
  }

  // BlockStatement implementation
  BlockStatement::BlockStatement(Token::Token token)
      : token(std::move(token)) {}

  void BlockStatement::statementNode() {}

  std::string BlockStatement::TokenLiteral() const
  {
    return token.literal;
  }

  std::string BlockStatement::String() const
  {
    std::string out = "{ ";
    for (const auto& stmt : statements)
    {
      if (stmt)
      {
        out += stmt->String();
      }
    }
    out += " }";
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
    std::string out = TokenLiteral() + " ";
    if (name)
    {
      out += name->String();
    }
    out += " = ";
    if (value)
    {
      out += value->String();
    }
    out += ";";
    return out;
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
    std::string out = TokenLiteral() + " ";
    if (returnValue)
    {
      out += returnValue->String();
    }
    out += ";";
    return out;
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
    return expression ? expression->String() : "";
  }

  // IntegerLiteral implementation
  IntegerLiteral::IntegerLiteral(Token::Token token, int64_t value)
      : token(std::move(token)), value(value) {}

  void IntegerLiteral::expressionNode() {}

  std::string IntegerLiteral::TokenLiteral() const
  {
    return token.literal;
  }

  std::string IntegerLiteral::String() const
  {
    return token.literal;
  }

  // PrefixExpression implementation
  PrefixExpression::PrefixExpression(Token::Token token, std::string op)
      : token(std::move(token)), op(std::move(op)) {}

  void PrefixExpression::expressionNode() {}

  std::string PrefixExpression::TokenLiteral() const
  {
    return token.literal;
  }

  std::string PrefixExpression::String() const
  {
    return "(" + op + right->String() + ")";
  }

  // InfixExpression implementation
  InfixExpression::InfixExpression(Token::Token token, std::string op, std::unique_ptr<Expression> left)
      : token(std::move(token)), op(std::move(op)), left(std::move(left)) {}

  void InfixExpression::expressionNode() {}

  std::string InfixExpression::TokenLiteral() const
  {
    return token.literal;
  }

  std::string InfixExpression::String() const
  {
    return "(" + left->String() + " " + op + " " + right->String() + ")";
  }

  // BooleanLiteral implementation
  BooleanLiteral::BooleanLiteral(Token::Token token, bool value)
      : token(std::move(token)), value(value) {}

  void BooleanLiteral::expressionNode() {}

  std::string BooleanLiteral::TokenLiteral() const
  {
    return token.literal;
  }

  std::string BooleanLiteral::String() const
  {
    return token.literal;
  }

  // FunctionLiteral implementation
  FunctionLiteral::FunctionLiteral(Token::Token token)
      : token(std::move(token)) {}

  void FunctionLiteral::expressionNode() {}

  std::string FunctionLiteral::TokenLiteral() const
  {
    return token.literal;
  }

  std::string FunctionLiteral::String() const
  {
    std::string out = token.literal + "(";
    for (size_t i = 0; i < parameters.size(); i++)
    {
      out += parameters[i]->String();
      if (i < parameters.size() - 1)
      {
        out += ", ";
      }
    }
    out += ") ";
    if (body)
    {
      out += body->String();
    }
    return out;
  }

  // CallExpression implementation
  CallExpression::CallExpression(Token::Token token, std::unique_ptr<Expression> function)
      : token(std::move(token)), function(std::move(function)) {}

  void CallExpression::expressionNode() {}

  std::string CallExpression::TokenLiteral() const
  {
    return token.literal;
  }

  std::string CallExpression::String() const
  {
    std::string out = function->String() + "(";
    for (size_t i = 0; i < arguments.size(); i++)
    {
      out += arguments[i]->String();
      if (i < arguments.size() - 1)
      {
        out += ", ";
      }
    }
    out += ")";
    return out;
  }

  // StringLiteral implementation
  StringLiteral::StringLiteral(Token::Token token, std::string value)
      : token(std::move(token)), value(std::move(value)) {}

  void StringLiteral::expressionNode() {}

  std::string StringLiteral::TokenLiteral() const
  {
    return token.literal;
  }

  std::string StringLiteral::String() const
  {
    return "\"" + value + "\"";
  }

  // ArrayLiteral実装
  ArrayLiteral::ArrayLiteral(Token::Token token) : token(std::move(token)) {}

  void ArrayLiteral::expressionNode() {}

  std::string ArrayLiteral::TokenLiteral() const {
      return token.literal;
  }

  std::string ArrayLiteral::String() const {
      std::string out = "[";
      for (size_t i = 0; i < elements.size(); ++i) {
          if (elements[i]) {
              out += elements[i]->String();
          }
          if (i < elements.size() - 1) {
              out += ", ";
          }
      }
      out += "]";
      return out;
  }

  // IndexExpression実装
  IndexExpression::IndexExpression(Token::Token token, std::unique_ptr<Expression> left)
      : token(std::move(token)), left(std::move(left)) {}

  void IndexExpression::expressionNode() {}

  std::string IndexExpression::TokenLiteral() const {
      return token.literal;
  }

  std::string IndexExpression::String() const {
      return "(" + left->String() + "[" + index->String() + "])";
  }
} // namespace AST
