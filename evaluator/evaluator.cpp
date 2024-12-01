#include "evaluator.hpp"
#include "../ast/ast.hpp"

namespace monkey
{

  ObjectPtr Evaluator::eval(const AST::Node *node)
  {
    if (node == nullptr)
    {
      return std::make_shared<Null>();
    }

    if (const auto *program = dynamic_cast<const AST::Program *>(node))
    {
      return evalProgram(program);
    }

    if (const auto *expressionStatement = dynamic_cast<const AST::ExpressionStatement *>(node))
    {
      return evalExpressionStatement(expressionStatement);
    }

    if (const auto *integerLiteral = dynamic_cast<const AST::IntegerLiteral *>(node))
    {
      return evalIntegerLiteral(integerLiteral);
    }
    if (const auto *booleanLiteral = dynamic_cast<const AST::BooleanLiteral *>(node))
    {
      return evalBooleanLiteral(booleanLiteral);
    }
    if (const auto *stringLiteral = dynamic_cast<const AST::StringLiteral *>(node))
    {
      return evalStringLiteral(stringLiteral);
    }
    if (const auto *prefixExpression = dynamic_cast<const AST::PrefixExpression *>(node))
    {
      return evalPrefixExpression(prefixExpression);
    }
    if (const auto *infixExpression = dynamic_cast<const AST::InfixExpression *>(node))
    {
      return evalInfixExpression(infixExpression);
    }

    return std::make_shared<Null>();
  }

  ObjectPtr Evaluator::evalProgram(const AST::Program *program)
  {
    if (!program)
      return std::make_shared<Null>();

    ObjectPtr result = std::make_shared<Null>();

    for (const auto &statement : program->statements)
    {
      result = evalStatement(statement.get());

      if (auto error = std::dynamic_pointer_cast<Error>(result))
      {
        return error;
      }
    }

    return result;
  }

  ObjectPtr Evaluator::evalStatement(const AST::Statement *statement)
  {
    if (!statement)
      return std::make_shared<Null>();

    if (const auto *expressionStatement = dynamic_cast<const AST::ExpressionStatement *>(statement))
    {
      return evalExpressionStatement(expressionStatement);
    }

    return std::make_shared<Null>();
  }

  ObjectPtr Evaluator::evalExpressionStatement(const AST::ExpressionStatement *statement)
  {
    if (!statement)
      return std::make_shared<Null>();
    return eval(statement->expression.get());
  }

  ObjectPtr Evaluator::evalIntegerLiteral(const AST::IntegerLiteral *node)
  {
    if (!node)
      return std::make_shared<Null>();
    return std::make_shared<Integer>(node->value);
  }

  ObjectPtr Evaluator::evalBooleanLiteral(const AST::BooleanLiteral *node)
  {
    if (!node)
      return std::make_shared<Null>();
    return std::make_shared<Boolean>(node->value);
  }

  ObjectPtr Evaluator::evalPrefixExpression(const AST::PrefixExpression *node)
  {
    if (!node)
      return std::make_shared<Null>();

    auto right = eval(node->right.get());
    if (!right)
      return std::make_shared<Null>();

    if (node->op == "!")
    {
      return evalBangOperatorExpression(right);
    }
    if (node->op == "-")
    {
      return evalMinusPrefixOperatorExpression(right);
    }

    return newError("unknown operator: " + node->op);
  }

  ObjectPtr Evaluator::evalInfixExpression(const AST::InfixExpression *node)
  {
    if (!node)
      return std::make_shared<Null>();

    auto left = eval(node->left.get());
    auto right = eval(node->right.get());

    if (!left || !right)
      return std::make_shared<Null>();

    if (std::dynamic_pointer_cast<Integer>(left) && std::dynamic_pointer_cast<Integer>(right))
    {
      return evalIntegerInfixExpression(node->op, left, right);
    }

    if (std::dynamic_pointer_cast<String>(left) && std::dynamic_pointer_cast<String>(right))
    {
      return evalStringInfixExpression(node->op, left, right);
    }

    return newError("type mismatch: " + left->inspect() + " " + node->op + " " + right->inspect());
  }

  ObjectPtr Evaluator::evalBangOperatorExpression(const ObjectPtr &right)
  {
    if (auto boolean = std::dynamic_pointer_cast<Boolean>(right))
    {
      return std::make_shared<Boolean>(!boolean->value());
    }
    if (std::dynamic_pointer_cast<Null>(right))
    {
      return std::make_shared<Boolean>(true);
    }
    return std::make_shared<Boolean>(false);
  }

  ObjectPtr Evaluator::evalMinusPrefixOperatorExpression(const ObjectPtr &right)
  {
    if (auto integer = std::dynamic_pointer_cast<Integer>(right))
    {
      return std::make_shared<Integer>(-integer->value());
    }
    return newError("invalid operator: -" + right->inspect());
  }

  ObjectPtr Evaluator::evalIntegerInfixExpression(const std::string &op, const ObjectPtr &left, const ObjectPtr &right)
  {
    auto leftVal = std::dynamic_pointer_cast<Integer>(left)->value();
    auto rightVal = std::dynamic_pointer_cast<Integer>(right)->value();

    if (op == "+")
      return std::make_shared<Integer>(leftVal + rightVal);
    if (op == "-")
      return std::make_shared<Integer>(leftVal - rightVal);
    if (op == "*")
      return std::make_shared<Integer>(leftVal * rightVal);
    if (op == "/")
    {
      if (rightVal == 0)
        return newError("division by zero");
      return std::make_shared<Integer>(leftVal / rightVal);
    }
    if (op == "<")
      return std::make_shared<Boolean>(leftVal < rightVal);
    if (op == ">")
      return std::make_shared<Boolean>(leftVal > rightVal);
    if (op == "==")
      return std::make_shared<Boolean>(leftVal == rightVal);
    if (op == "!=")
      return std::make_shared<Boolean>(leftVal != rightVal);

    return newError("unknown operator: " + op);
  }

  ObjectPtr Evaluator::evalStringInfixExpression(const std::string &op, const ObjectPtr &left, const ObjectPtr &right)
  {
    auto leftVal = std::dynamic_pointer_cast<String>(left)->value();
    auto rightVal = std::dynamic_pointer_cast<String>(right)->value();

    if (op == "+")
    {
      return std::make_shared<String>(leftVal + rightVal);
    }
    if (op == "==")
    {
      return std::make_shared<Boolean>(leftVal == rightVal);
    }
    if (op == "!=")
    {
      return std::make_shared<Boolean>(leftVal != rightVal);
    }

    return newError("unknown operator: " + op);
  }

  ObjectPtr Evaluator::newError(const std::string &message)
  {
    return std::make_shared<Error>(message);
  }

} // namespace monkey
