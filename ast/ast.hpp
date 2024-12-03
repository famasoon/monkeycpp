#pragma once
#include "../token/token.hpp"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

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
    virtual Statement *clone() const = 0;
};

class Expression : public Node
{
  public:
    virtual ~Expression() = default;
    virtual void expressionNode() = 0;
    virtual Expression *clone() const = 0;
};

// 式文
class ExpressionStatement : public Statement
{
  public:
    Token::Token token;
    std::unique_ptr<Expression> expression;

    explicit ExpressionStatement(Token::Token token);
    ExpressionStatement(const ExpressionStatement &other);
    void statementNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
    Statement *clone() const override;
};

// ブロック文
class BlockStatement : public Statement
{
  public:
    Token::Token token;
    std::vector<std::unique_ptr<Statement>> statements;

    explicit BlockStatement(Token::Token token);
    BlockStatement(const BlockStatement &other);
    void statementNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
    Statement *clone() const override;
};

// プログラム全体を表すクラス（Statementクラスの後に移動）
class Program : public Node
{
  public:
    std::vector<std::unique_ptr<Statement>> statements;

    std::string TokenLiteral() const override;
    std::string String() const override;
    void clearStatements();
    void addStatement(std::unique_ptr<Statement> stmt);
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
    Expression *clone() const override;
    
    const std::string& getValue() const { return value; }
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
    Statement *clone() const override;
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
    Statement *clone() const override;
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
    Expression *clone() const override;
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
    Expression *clone() const override;
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
    Expression *clone() const override;
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
    Expression *clone() const override;
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
    Expression *clone() const override;
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
    Expression *clone() const override;
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

    const std::string &getValue() const
    {
        return value;
    }
    Expression *clone() const override;
};

// 配列リテラル
class ArrayLiteral : public Expression
{
  public:
    Token::Token token; // '['トークン
    std::vector<std::unique_ptr<Expression>> elements;

    explicit ArrayLiteral(Token::Token token);
    void expressionNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
    Expression *clone() const override;
};

// インデックス式
class IndexExpression : public Expression
{
  public:
    Token::Token token; // '['トークン
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> index;

    IndexExpression(Token::Token token, std::unique_ptr<Expression> left);
    void expressionNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
    Expression *clone() const override;
};

// ハッシュリテラル
class HashLiteral : public Expression
{
  public:
    Token::Token token; // '{'トークン
    std::unordered_map<std::unique_ptr<Expression>, std::unique_ptr<Expression>> pairs;

    explicit HashLiteral(Token::Token tok);
    void expressionNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
    Expression *clone() const override;
};

class IfExpression : public Expression {
private:
    Token::Token token;  // 'if'トークン
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStatement> consequence;
    std::unique_ptr<BlockStatement> alternative;

public:
    IfExpression(Token::Token tok,
                 std::unique_ptr<Expression> cond,
                 std::unique_ptr<BlockStatement> cons,
                 std::unique_ptr<BlockStatement> alt = nullptr)
        : token(std::move(tok)), 
          condition(std::move(cond)), 
          consequence(std::move(cons)), 
          alternative(std::move(alt)) {}

    void expressionNode() override {}
    std::string TokenLiteral() const override { return token.getLiteral(); }
    std::string String() const override {
        std::string result = "if" + condition->String() + " " + consequence->String();
        if (alternative) {
            result += "else " + alternative->String();
        }
        return result;
    }
    Expression* clone() const override {
        return new IfExpression(
            token,
            std::unique_ptr<Expression>(condition->clone()),
            std::unique_ptr<BlockStatement>(static_cast<BlockStatement*>(consequence->clone())),
            alternative ? std::unique_ptr<BlockStatement>(static_cast<BlockStatement*>(alternative->clone())) : nullptr
        );
    }

    const Expression* getCondition() const { return condition.get(); }
    const BlockStatement* getConsequence() const { return consequence.get(); }
    const BlockStatement* getAlternative() const { return alternative.get(); }
};

class WhileExpression : public Expression {
private:
    Token::Token token;

public:
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStatement> body;

    explicit WhileExpression(Token::Token tok);
    WhileExpression(Token::Token tok,
                    std::unique_ptr<Expression> cond,
                    std::unique_ptr<BlockStatement> b);
    void expressionNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
    Expression* clone() const override;

    const Expression* getCondition() const { return condition.get(); }
    const BlockStatement* getBody() const { return body.get(); }
};

class ForExpression : public Expression {
private:
    Token::Token token;  // 'for'トークン

public:
    std::unique_ptr<Expression> init;      // 初期化式
    std::unique_ptr<Expression> condition; // 条件式
    std::unique_ptr<Expression> update;    // 更新式
    std::unique_ptr<BlockStatement> body;  // 本体

    explicit ForExpression(Token::Token tok);
    ForExpression(Token::Token tok,
                 std::unique_ptr<Expression> init,
                 std::unique_ptr<Expression> cond,
                 std::unique_ptr<Expression> update,
                 std::unique_ptr<BlockStatement> b);
    void expressionNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
    Expression* clone() const override;
};

class LetExpression : public Expression {
private:
    Token::Token token;
    std::unique_ptr<Identifier> name;
    std::unique_ptr<Expression> value;

public:
    LetExpression(Token::Token tok,
                  std::unique_ptr<Identifier> name,
                  std::unique_ptr<Expression> value);
    void expressionNode() override;
    std::string TokenLiteral() const override;
    std::string String() const override;
    Expression* clone() const override;
};

} // namespace AST
