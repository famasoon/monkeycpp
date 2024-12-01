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

    std::string TokenLiteral() const override;
    std::string String() const override;
  };

  // 識別子を表すクラス
  class Identifier : public Expression
  {
  public:
    Token::Token token;
    std::string value;

    Identifier(Token::Token token, std::string value);

    void expressionNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
  };

  // let文を表すクラス
  class LetStatement : public Statement
  {
  public:
    Token::Token token;
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Expression> value;

    LetStatement(Token::Token token);

    void statementNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
  };

  // return文を表すクラス
  class ReturnStatement : public Statement
  {
  public:
    Token::Token token;
    std::unique_ptr<Expression> returnValue;

    ReturnStatement(Token::Token token);

    void statementNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
  };

  // 式文を表すクラス
  class ExpressionStatement : public Statement
  {
  public:
    Token::Token token;
    std::unique_ptr<Expression> expression;

    ExpressionStatement(Token::Token token);

    void statementNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
  };

} // namespace AST
