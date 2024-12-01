#pragma once
#include "../object/object.hpp"
#include "../ast/ast.hpp"
#include <memory>

namespace monkey
{

  class Evaluator
  {
  public:
    Evaluator();
    ObjectPtr eval(const AST::Node *node);
    void collectGarbage();
    EnvPtr getEnv() const { return env; }
    void setEnv(EnvPtr newEnv) { env = std::move(newEnv); }

  private:
    EnvPtr env;

    // プログラムと文の評価
    ObjectPtr evalProgram(const AST::Program *program);
    ObjectPtr evalBlockStatement(const AST::BlockStatement *block);
    ObjectPtr evalLetStatement(const AST::LetStatement *letStmt);
    ObjectPtr evalReturnStatement(const AST::ReturnStatement *returnStmt);
    ObjectPtr evalExpressionStatement(const AST::ExpressionStatement *exprStmt);

    // 式の評価
    ObjectPtr evalPrefixExpression(const AST::PrefixExpression *expr);
    ObjectPtr evalInfixExpression(const AST::InfixExpression *expr);
    ObjectPtr evalCallExpression(const AST::CallExpression *call);

    // リテラルの評価
    ObjectPtr evalIntegerLiteral(const AST::IntegerLiteral *node);
    ObjectPtr evalBooleanLiteral(const AST::BooleanLiteral *node);
    ObjectPtr evalStringLiteral(const AST::StringLiteral *node);
    ObjectPtr evalFunctionLiteral(const AST::FunctionLiteral *node);
    ObjectPtr evalIdentifier(const AST::Identifier *node);

    // 演算子の評価
    ObjectPtr evalBangOperatorExpression(const ObjectPtr &right);
    ObjectPtr evalMinusPrefixOperatorExpression(const ObjectPtr &right);
    ObjectPtr evalInfixOperation(const std::string &op,
                                 const ObjectPtr &left,
                                 const ObjectPtr &right);

    // 型別の中置演算子の評価
    ObjectPtr evalIntegerInfixExpression(
        const std::string &op,
        const std::shared_ptr<Integer> &left,
        const std::shared_ptr<Integer> &right);

    ObjectPtr evalBooleanInfixExpression(
        const std::string &op,
        const std::shared_ptr<Boolean> &left,
        const std::shared_ptr<Boolean> &right);

    ObjectPtr evalStringInfixExpression(
        const std::string &op,
        const std::shared_ptr<String> &left,
        const std::shared_ptr<String> &right);

    // エラーハンドリング
    ObjectPtr newError(const std::string &message);
    std::string objectTypeToString(ObjectType type);

    // 配列関連の評価
    ObjectPtr evalArrayLiteral(const AST::ArrayLiteral* array);
    ObjectPtr evalIndexExpression(const AST::IndexExpression* indexExpr);
    ObjectPtr evalArrayIndexExpression(const ObjectPtr& array, const ObjectPtr& index);
  };

} // namespace monkey