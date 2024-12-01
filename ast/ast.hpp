#pragma once
#include "../token/token.hpp"
#include <string>
#include <vector>
#include <memory>

namespace AST
{
  // 基本インターフェース
  class Node
  {
  public:
    virtual ~Node() = default;
    virtual std::string TokenLiteral() const = 0;
    virtual std::string String() const = 0;
  };

  class Statement : public Node
  {
  public:
    virtual ~Statement() = default;
    virtual void statementNode() = 0;
  };

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

  // ブロック文
  class BlockStatement : public Statement
  {
  public:
    Token::Token token;
    std::vector<std::unique_ptr<Statement>> statements;

    explicit BlockStatement(Token::Token token);
    void statementNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
  };

  // 識別子
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

  // let文
  class LetStatement : public Statement
  {
  public:
    Token::Token token;
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Expression> value;

    explicit LetStatement(Token::Token token);
    void statementNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
  };

  // return文
  class ReturnStatement : public Statement
  {
  public:
    Token::Token token;
    std::unique_ptr<Expression> returnValue;

    explicit ReturnStatement(Token::Token token);
    void statementNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
  };

  // 式文
  class ExpressionStatement : public Statement
  {
  public:
    Token::Token token;
    std::unique_ptr<Expression> expression;

    explicit ExpressionStatement(Token::Token token);
    void statementNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
  };

  // 整数リテラル
  class IntegerLiteral : public Expression
  {
  public:
    Token::Token token;
    int64_t value;

    IntegerLiteral(Token::Token token, int64_t value);
    void expressionNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
  };

  // 前置式
  class PrefixExpression : public Expression
  {
  public:
    Token::Token token;
    std::string op;
    std::unique_ptr<Expression> right;

    PrefixExpression(Token::Token token, std::string op);
    void expressionNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
  };

  // 中置式
  class InfixExpression : public Expression
  {
  public:
    Token::Token token;
    std::string op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    InfixExpression(Token::Token token, std::string op, std::unique_ptr<Expression> left);
    void expressionNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
  };

  // 真偽値リテラル
  class BooleanLiteral : public Expression
  {
  public:
    Token::Token token;
    bool value;

    BooleanLiteral(Token::Token token, bool value);
    void expressionNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
  };

  // 関数リテラル
  class FunctionLiteral : public Expression
  {
  public:
    Token::Token token;
    std::vector<std::unique_ptr<Identifier>> parameters;
    std::unique_ptr<BlockStatement> body;

    explicit FunctionLiteral(Token::Token token);
    void expressionNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
  };

  // 呼び出し式
  class CallExpression : public Expression
  {
  public:
    Token::Token token;
    std::unique_ptr<Expression> function;
    std::vector<std::unique_ptr<Expression>> arguments;

    CallExpression(Token::Token token, std::unique_ptr<Expression> function);
    void expressionNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
  };

  // 文字列リテラル
  class StringLiteral : public Expression
  {
  private:
    Token::Token token;
    std::string value;

  public:
    StringLiteral(Token::Token token, std::string value);
    void expressionNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
    
    const std::string& getValue() const { return value; }
  };

  enum class ExpressionType {
    // 既存の型...
    StringLiteral,
    // その他の型...
  };

} // namespace AST
