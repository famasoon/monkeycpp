#pragma once
#include "../ast/ast.hpp"
#include "../object/object.hpp"
#include <memory>

namespace monkey
{

class Evaluator
{
  public:
    Evaluator();
    ObjectPtr eval(const AST::Node *node);
    void collectGarbage();
    EnvPtr getEnv() const;
    void setEnv(EnvPtr newEnv);

  private:
    EnvPtr env;

    // ヘルパー関数
    ObjectPtr newError(const std::string& message);
    bool isError(const ObjectPtr& obj);
    std::string objectTypeToString(ObjectType type);
    bool isTruthy(const ObjectPtr& obj);
    
    // 評価関数
    ObjectPtr evalProgram(const AST::Program* program);
    ObjectPtr evalBlockStatement(const AST::BlockStatement* block);
    ObjectPtr evalExpressionStatement(const AST::ExpressionStatement* exprStmt);
    ObjectPtr evalLetStatement(const AST::LetStatement* letStmt);
    ObjectPtr evalReturnStatement(const AST::ReturnStatement* returnStmt);
    
    // リテラルの評価
    ObjectPtr evalIntegerLiteral(const AST::IntegerLiteral* node);
    ObjectPtr evalBooleanLiteral(const AST::BooleanLiteral* node);
    ObjectPtr evalStringLiteral(const AST::StringLiteral* node);
    ObjectPtr evalFunctionLiteral(const AST::FunctionLiteral* node);
    ObjectPtr evalIdentifier(const AST::Identifier* node);
    
    // 式の評価
    ObjectPtr evalPrefixExpression(const std::string& op, ObjectPtr right);
    ObjectPtr evalInfixExpression(const std::string& op, ObjectPtr left, ObjectPtr right);
    ObjectPtr evalIntegerInfixExpression(const std::string& op, 
                                       std::shared_ptr<Integer> left,
                                       std::shared_ptr<Integer> right);
    ObjectPtr evalBangOperatorExpression(const ObjectPtr& right);
    ObjectPtr evalCallExpression(const AST::CallExpression* call);
    
    // 配列とハッシュの評価
    ObjectPtr evalArrayLiteral(const AST::ArrayLiteral* array);
    ObjectPtr evalIndexExpression(const AST::IndexExpression* index);
    ObjectPtr evalArrayIndexExpression(const ObjectPtr& array, const ObjectPtr& index);
    ObjectPtr evalHashLiteral(const AST::HashLiteral* hash);
    ObjectPtr evalHashIndexExpression(const ObjectPtr& hash, const ObjectPtr& index);
};

} // namespace monkey