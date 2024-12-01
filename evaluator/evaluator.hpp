#ifndef EVALUATOR_HPP
#define EVALUATOR_HPP

#include "../object/object.hpp"
#include "../ast/ast.hpp"
#include <memory>
#include <string>

namespace monkey
{
  class Evaluator
  {
  public:
    Evaluator() : env(Environment::NewEnvironment()) {}

    ObjectPtr eval(const AST::Node *node);
    void collectGarbage()
    {
      if (env)
        env->MarkAndSweep();
    }

    EnvPtr getEnv() const { return env; }
    void setEnv(EnvPtr newEnv) { env = std::move(newEnv); }

  private:
    EnvPtr env; // 評価器の環境

    // プログラムと式の評価
    ObjectPtr evalProgram(const AST::Program *program);
    ObjectPtr evalPrefixExpression(const AST::PrefixExpression *expr);
    ObjectPtr evalInfixExpression(const AST::InfixExpression *expr);

    // リテラルの評価
    ObjectPtr evalIntegerLiteral(const AST::IntegerLiteral *node);
    ObjectPtr evalBooleanLiteral(const AST::BooleanLiteral *node);
    ObjectPtr evalStringLiteral(const AST::StringLiteral *node);
    ObjectPtr evalFunctionLiteral(const AST::FunctionLiteral *node);
    ObjectPtr evalIdentifier(const AST::Identifier *node);

    // 文の評価
    ObjectPtr evalBlockStatement(const AST::BlockStatement *block);
    ObjectPtr evalLetStatement(const AST::LetStatement *letStmt);
    ObjectPtr evalReturnStatement(const AST::ReturnStatement *returnStmt);
    ObjectPtr evalExpressionStatement(const AST::ExpressionStatement *exprStmt);
    ObjectPtr evalCallExpression(const AST::CallExpression *call);

    // 演算子の評価
    ObjectPtr evalBangOperatorExpression(const ObjectPtr &right);
    ObjectPtr evalMinusPrefixOperatorExpression(const ObjectPtr &right);
    ObjectPtr evalInfixOperation(const std::string &op, const ObjectPtr &left, const ObjectPtr &right);

    // 型別の中置演算子の評価
    ObjectPtr evalIntegerInfixExpression(const std::string &op,
                                         const std::shared_ptr<Integer> &left,
                                         const std::shared_ptr<Integer> &right);
    ObjectPtr evalBooleanInfixExpression(const std::string &op,
                                         const std::shared_ptr<Boolean> &left,
                                         const std::shared_ptr<Boolean> &right);
    ObjectPtr evalStringInfixExpression(const std::string &op,
                                        const std::shared_ptr<String> &left,
                                        const std::shared_ptr<String> &right);

    // ユーティリティ
    std::string objectTypeToString(ObjectType type);
    ObjectPtr newError(const std::string &message);
  };

} // namespace monkey

#endif // EVALUATOR_HPP