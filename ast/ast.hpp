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

  // ブロック文を先に定義
  class BlockStatement : public Statement
  {
  public:
    Token::Token token; // '{' トークン
    std::vector<std::unique_ptr<Statement>> statements;

    BlockStatement(Token::Token token)
        : token(std::move(token)) {}

    void statementNode() override {}
    std::string TokenLiteral() const override { return token.literal; }
    std::string String() const override
    {
      std::string out = "{ ";
      for (const auto &stmt : statements)
      {
        if (stmt)
        {
          out += stmt->String();
        }
      }
      out += " }";
      return out;
    }
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

  class IntegerLiteral : public Expression
  {
  public:
    Token::Token token;
    int64_t value;

    IntegerLiteral(Token::Token token, int64_t value)
        : token(std::move(token)), value(value) {}

    void expressionNode() override {}
    std::string TokenLiteral() const override { return token.literal; }
    std::string String() const override { return token.literal; }
  };

  class PrefixExpression : public Expression
  {
  public:
    Token::Token token;
    std::string op;
    std::unique_ptr<Expression> right;

    PrefixExpression(Token::Token token, std::string op)
        : token(std::move(token)), op(std::move(op)) {}

    void expressionNode() override {}
    std::string TokenLiteral() const override { return token.literal; }
    std::string String() const override
    {
      return "(" + op + right->String() + ")";
    }
  };

  class InfixExpression : public Expression
  {
  public:
    Token::Token token;
    std::string op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

    InfixExpression(Token::Token token, std::string op, std::unique_ptr<Expression> left)
        : token(std::move(token)), op(std::move(op)), left(std::move(left)) {}

    void expressionNode() override {}
    std::string TokenLiteral() const override { return token.literal; }
    std::string String() const override
    {
      return "(" + left->String() + op + right->String() + ")";
    }
  };

  // Function リテラルを最後に定義
  class FunctionLiteral : public Expression
  {
  public:
    Token::Token token;
    std::vector<std::unique_ptr<Identifier>> parameters;
    std::unique_ptr<BlockStatement> body;

    FunctionLiteral(Token::Token token)
        : token(std::move(token)) {}

    void expressionNode() override {}
    std::string TokenLiteral() const override { return token.literal; }
    std::string String() const override
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
  };

} // namespace AST
