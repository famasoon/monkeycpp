#pragma once
#include "../token/token.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace AST
{
  // 前方宣言
  class Expression;
  class Statement;
  using ExpressionPtr = std::unique_ptr<Expression>;

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
    virtual Statement* clone() const = 0;
  };

  class Expression : public Node
  {
  public:
    virtual ~Expression() = default;
    virtual void expressionNode() = 0;
    virtual Expression* clone() const = 0;
  };

  // 式文を先に定義
  class ExpressionStatement : public Statement
  {
  public:
    Token::Token token;
    std::unique_ptr<Expression> expression;

    explicit ExpressionStatement(Token::Token token);
    ExpressionStatement(const ExpressionStatement& other)
        : token(other.token)
        , expression(other.expression ? std::unique_ptr<Expression>(other.expression->clone()) : nullptr) {}
    
    void statementNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
    Statement* clone() const override {
        return new ExpressionStatement(*this);
    }
  };

  // ブロック文
  class BlockStatement : public Statement
  {
  public:
    Token::Token token;
    std::vector<std::unique_ptr<Statement>> statements;

    explicit BlockStatement(Token::Token token);
    BlockStatement(const BlockStatement& other) : token(other.token) {
        statements.reserve(other.statements.size());
        for (const auto& stmt : other.statements) {
            if (stmt) {
                statements.push_back(std::unique_ptr<Statement>(stmt->clone()));
            }
        }
    }
    
    void statementNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
    Statement* clone() const override {
        return new BlockStatement(*this);
    }
  };

  // プログラム全体を表すクラス（Statementクラスの後に移動）
  class Program : public Node
  {
  public:
    std::vector<std::unique_ptr<Statement>> statements;

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
    Expression* clone() const override {
        return new Identifier(token, value);
    }
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
    Statement* clone() const override {
        auto cloned = new LetStatement(token);
        if (name) {
            cloned->name.reset(static_cast<Identifier*>(name->clone()));
        }
        if (value) {
            cloned->value.reset(value->clone());
        }
        return cloned;
    }
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
    Statement* clone() const override {
        auto cloned = new ReturnStatement(token);
        if (returnValue) {
            cloned->returnValue.reset(returnValue->clone());
        }
        return cloned;
    }
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
    Expression* clone() const override {
        return new IntegerLiteral(token, value);
    }
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
    Expression* clone() const override;
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
    Expression* clone() const override;
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
    Expression* clone() const override {
        return new BooleanLiteral(token, value);
    }
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
    Expression* clone() const override {
        auto cloned = new FunctionLiteral(token);
        for (const auto& param : parameters) {
            if (param) {
                cloned->parameters.push_back(std::unique_ptr<Identifier>(
                    static_cast<Identifier*>(param->clone())));
            }
        }
        if (body) {
            cloned->body.reset(static_cast<BlockStatement*>(body->clone()));
        }
        return cloned;
    }
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
    Expression* clone() const override {
        auto cloned = new CallExpression(token, nullptr);
        if (function) {
            cloned->function.reset(function->clone());
        }
        for (const auto& arg : arguments) {
            if (arg) {
                cloned->arguments.push_back(std::unique_ptr<Expression>(arg->clone()));
            }
        }
        return cloned;
    }
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
    Expression* clone() const override {
        return new StringLiteral(token, value);
    }
  };

  // 配列リテラル
  class ArrayLiteral : public Expression {
  public:
    Token::Token token;  // '['トークン
    std::vector<std::unique_ptr<Expression>> elements;

    explicit ArrayLiteral(Token::Token token);
    void expressionNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
    Expression* clone() const override {
        auto cloned = new ArrayLiteral(token);
        for (const auto& elem : elements) {
            if (elem) {
                cloned->elements.push_back(std::unique_ptr<Expression>(elem->clone()));
            }
        }
        return cloned;
    }
  };

  // インデックス式
  class IndexExpression : public Expression {
  public:
    Token::Token token;  // '['トークン
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> index;

    IndexExpression(Token::Token token, std::unique_ptr<Expression> left);
    void expressionNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
    Expression* clone() const override {
        auto cloned = new IndexExpression(token, nullptr);
        if (left) {
            cloned->left.reset(left->clone());
        }
        if (index) {
            cloned->index.reset(index->clone());
        }
        return cloned;
    }
  };

  // ハッシュリテラル
  class HashLiteral : public Expression {
  public:
    Token::Token token; // '{'トークン
    std::unordered_map<std::unique_ptr<Expression>, std::unique_ptr<Expression>> pairs;

    explicit HashLiteral(Token::Token tok);
    void expressionNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
    Expression* clone() const override {
        auto cloned = new HashLiteral(token);
        for (const auto& pair : pairs) {
            if (pair.first && pair.second) {
                cloned->pairs[std::unique_ptr<Expression>(pair.first->clone())] = 
                    std::unique_ptr<Expression>(pair.second->clone());
            }
        }
        return cloned;
    }
  };

} // namespace AST
