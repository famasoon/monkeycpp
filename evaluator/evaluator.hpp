#pragma once
#include "../ast/ast.hpp"
#include "../object/object.hpp"
#include <memory>
#include <string>

namespace monkey
{

  class Evaluator
  {
  public:
    Evaluator() = default;
    ObjectPtr eval(const AST::Node* node);

  private:
    ObjectPtr evalProgram(const AST::Program* program);
    ObjectPtr evalStatement(const AST::Statement* statement);
    ObjectPtr evalExpressionStatement(const AST::ExpressionStatement* statement);

    ObjectPtr evalIntegerLiteral(const AST::IntegerLiteral* node);
    ObjectPtr evalStringLiteral(const AST::StringLiteral* node);
    ObjectPtr evalBooleanLiteral(const AST::BooleanLiteral* node);
    ObjectPtr evalPrefixExpression(const std::string& op, const ObjectPtr& right);
    ObjectPtr evalInfixExpression(const std::string& op, const ObjectPtr& left, const ObjectPtr& right);
    
    ObjectPtr evalBangOperatorExpression(const ObjectPtr& right);
    ObjectPtr evalMinusPrefixOperatorExpression(const ObjectPtr& right);
    ObjectPtr evalEqualityExpression(const std::string& op, const ObjectPtr& left, const ObjectPtr& right);
    
    ObjectPtr evalIntegerInfixExpression(const std::string& op, const ObjectPtr& left, const ObjectPtr& right);
    ObjectPtr evalStringInfixExpression(const std::string& op, const ObjectPtr& left, const ObjectPtr& right);
    
    ObjectPtr newError(const std::string& message);
    std::string objectTypeToString(ObjectType type);
  };

} // namespace monkey