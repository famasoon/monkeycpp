#pragma once
#include "../token/token.hpp"
#include <string>
#include <vector>
#include <memory>

namespace AST
{

  // 全てのノードのベースとなるインターフェース
  class Node
  {
  public:
    virtual ~Node() = default;
    virtual std::string TokenLiteral() const = 0;
    virtual std::string String() const = 0;
  };

  // 文のインターフェース
  class Statement : public Node
  {
  public:
    virtual ~Statement() = default;
    virtual void statementNode() = 0;
  };

  // 式のインターフェース
  class Expression : public Node
  {
  public:
    virtual ~Expression() = default;
    virtual void expressionNode() = 0;
  };

  // プログラム全体を表すクラス
  class Program : public Node
  {
  public:
    std::vector<std::unique_ptr<Statement>> statements;

    std::string TokenLiteral() const override
    {
      if (!statements.empty())
      {
        return statements[0]->TokenLiteral();
      }
      return "";
    }

    std::string String() const override
    {
      std::string result;
      for (const auto &stmt : statements)
      {
        result += stmt->String();
      }
      return result;
    }
  };

  // 識別子を表すクラス
  class Identifier : public Expression
  {
  public:
    Token::Token token;
    std::string value;

    Identifier(Token::Token token, std::string value)
        : token(std::move(token)), value(std::move(value)) {}

    void expressionNode() override {}
    std::string TokenLiteral() const override { return token.literal; }
    std::string String() const override { return value; }
  };

  // let文を表すクラス
  class LetStatement : public Statement
  {
  public:
    Token::Token token;
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Expression> value;

    LetStatement(Token::Token token)
        : token(std::move(token)) {}

    void statementNode() override {}
    std::string TokenLiteral() const override { return token.literal; }
    std::string String() const override
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
  };

  // return文を表すクラス
  class ReturnStatement : public Statement
  {
  public:
    Token::Token token;
    std::unique_ptr<Expression> returnValue;

    ReturnStatement(Token::Token token)
        : token(std::move(token)) {}

    void statementNode() override {}
    std::string TokenLiteral() const override { return token.literal; }
    std::string String() const override
    {
      std::string result = TokenLiteral() + " ";
      if (returnValue)
      {
        result += returnValue->String();
      }
      result += ";";
      return result;
    }
  };

  // 式文を表すクラス
  class ExpressionStatement : public Statement
  {
  public:
    Token::Token token;
    std::unique_ptr<Expression> expression;

    ExpressionStatement(Token::Token token)
        : token(std::move(token)) {}

    void statementNode() override {}
    std::string TokenLiteral() const override { return token.literal; }
    std::string String() const override
    {
      if (expression)
      {
        return expression->String();
      }
      return "";
    }
  };

} // namespace AST
