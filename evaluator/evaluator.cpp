#include "evaluator.hpp"
#include "../ast/ast.hpp"
#include <iostream>

namespace monkey
{

  void debugPrintAST(const AST::Node *node, int indent = 0)
  {
    std::string indentStr(indent * 2, ' ');

    if (const auto *program = dynamic_cast<const AST::Program *>(node))
    {
      std::cout << indentStr << "Program {\n";
      for (const auto &stmt : program->statements)
      {
        debugPrintAST(stmt.get(), indent + 1);
      }
      std::cout << indentStr << "}\n";
    }
    else if (const auto *expr = dynamic_cast<const AST::ExpressionStatement *>(node))
    {
      std::cout << indentStr << "ExpressionStatement {\n";
      if (expr->expression)
      {
        debugPrintAST(expr->expression.get(), indent + 1);
      }
      std::cout << indentStr << "}\n";
    }
    else if (const auto *infix = dynamic_cast<const AST::InfixExpression *>(node))
    {
      std::cout << indentStr << "InfixExpression {\n";
      std::cout << indentStr << "  operator: " << infix->op << "\n";
      std::cout << indentStr << "  left: ";
      if (infix->left)
        debugPrintAST(infix->left.get(), indent + 1);
      std::cout << indentStr << "  right: ";
      if (infix->right)
        debugPrintAST(infix->right.get(), indent + 1);
      std::cout << indentStr << "}\n";
    }
    else if (const auto *prefix = dynamic_cast<const AST::PrefixExpression *>(node))
    {
      std::cout << indentStr << "PrefixExpression {\n";
      std::cout << indentStr << "  operator: " << prefix->op << "\n";
      std::cout << indentStr << "  right: ";
      if (prefix->right)
        debugPrintAST(prefix->right.get(), indent + 1);
      std::cout << indentStr << "}\n";
    }
    else if (const auto *integer = dynamic_cast<const AST::IntegerLiteral *>(node))
    {
      std::cout << indentStr << "IntegerLiteral(" << integer->value << ")\n";
    }
    else if (const auto *boolean = dynamic_cast<const AST::BooleanLiteral *>(node))
    {
      std::cout << indentStr << "BooleanLiteral(" << (boolean->value ? "true" : "false") << ")\n";
    }
    else if (const auto *str = dynamic_cast<const AST::StringLiteral *>(node))
    {
      std::cout << indentStr << "StringLiteral(\"" << str->value << "\")\n";
    }
    else if (const auto *ident = dynamic_cast<const AST::Identifier *>(node))
    {
      std::cout << indentStr << "Identifier(" << ident->value << ")\n";
    }
    else
    {
      std::cout << indentStr << "Unknown Node Type: " << typeid(*node).name() << "\n";
    }
  }

  ObjectPtr Evaluator::eval(const AST::Node *node)
  {
    if (node == nullptr)
    {
      return std::make_shared<Null>();
    }

    // リテラルの評価を先に行う
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

    // ExpressionStatementの評価
    if (const auto *expressionStatement = dynamic_cast<const AST::ExpressionStatement *>(node))
    {
      if (!expressionStatement->expression)
      {
        return std::make_shared<Null>();
      }
      return eval(expressionStatement->expression.get());
    }

    // Programの評価
    if (const auto *program = dynamic_cast<const AST::Program *>(node))
    {
      return evalProgram(program);
    }

    // 式の評価
    if (const auto *prefixExpression = dynamic_cast<const AST::PrefixExpression *>(node))
    {
      if (!prefixExpression->right)
      {
        return newError("invalid prefix expression: right operand is null");
      }
      auto right = eval(prefixExpression->right.get());
      if (auto error = std::dynamic_pointer_cast<Error>(right))
      {
        return error;
      }
      return evalPrefixExpression(prefixExpression->op, right);
    }

    if (const auto *infixExpression = dynamic_cast<const AST::InfixExpression *>(node))
    {
      if (!infixExpression->left || !infixExpression->right)
      {
        return newError("invalid infix expression: operand is null");
      }

      // 左辺を先に評価
      auto left = eval(infixExpression->left.get());
      if (auto error = std::dynamic_pointer_cast<Error>(left))
      {
        return error;
      }

      // 右辺を評価
      auto right = eval(infixExpression->right.get());
      if (auto error = std::dynamic_pointer_cast<Error>(right))
      {
        return error;
      }

      // 演算子の優先順位に従って評価
      if (infixExpression->op == "*" || infixExpression->op == "/")
      {
        auto result = evalInfixExpression(infixExpression->op, left, right);
        if (auto error = std::dynamic_pointer_cast<Error>(result))
        {
          return error;
        }
        return result;
      }

      // 加減算は後で評価
      return evalInfixExpression(infixExpression->op, left, right);
    }

    return std::make_shared<Null>();
  }

  ObjectPtr Evaluator::evalProgram(const AST::Program *program)
  {
    if (!program)
      return std::make_shared<Null>();

    ObjectPtr result;
    for (const auto &statement : program->statements)
    {
      if (!statement)
        continue;
      result = eval(statement.get());

      if (auto error = std::dynamic_pointer_cast<Error>(result))
      {
        return error;
      }
    }
    return result ? result : std::make_shared<Null>();
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
    std::cout << "Evaluating boolean literal: " << (node->value ? "true" : "false") << "\n";
    return std::make_shared<Boolean>(node->value);
  }

  ObjectPtr Evaluator::evalStringLiteral(const AST::StringLiteral *node)
  {
    if (!node)
      return std::make_shared<Null>();
    std::cout << "Evaluating string literal: \"" << node->value << "\"\n";
    return std::make_shared<String>(node->value);
  }

  ObjectPtr Evaluator::evalPrefixExpression(const std::string &op, const ObjectPtr &right)
  {
    if (!right)
      return std::make_shared<Null>();

    if (op == "!")
    {
      return evalBangOperatorExpression(right);
    }
    if (op == "-")
    {
      if (auto integer = std::dynamic_pointer_cast<Integer>(right))
      {
        return std::make_shared<Integer>(-integer->value());
      }
      return newError("invalid operator: -" + objectTypeToString(right->type()));
    }

    return newError("unknown operator: " + op);
  }

  ObjectPtr Evaluator::evalInfixExpression(const std::string &op, const ObjectPtr &left, const ObjectPtr &right)
  {
    // 型が異なる場合のエラー処理を先に行う
    if (left->type() != right->type())
    {
      return newError("type mismatch: " + objectTypeToString(left->type()) + " " + 
                     op + " " + objectTypeToString(right->type()));
    }

    // 整数の演算
    if (auto leftInt = std::dynamic_pointer_cast<Integer>(left))
    {
      if (auto rightInt = std::dynamic_pointer_cast<Integer>(right))
      {
        auto leftVal = leftInt->value();
        auto rightVal = rightInt->value();

        // 演算子の優先順位に従って評価
        if (op == "*") return std::make_shared<Integer>(leftVal * rightVal);
        if (op == "/") {
          if (rightVal == 0) return newError("division by zero");
          return std::make_shared<Integer>(leftVal / rightVal);
        }
        if (op == "+") return std::make_shared<Integer>(leftVal + rightVal);
        if (op == "-") return std::make_shared<Integer>(leftVal - rightVal);
        if (op == "<") return std::make_shared<Boolean>(leftVal < rightVal);
        if (op == ">") return std::make_shared<Boolean>(leftVal > rightVal);
        if (op == "==") return std::make_shared<Boolean>(leftVal == rightVal);
        if (op == "!=") return std::make_shared<Boolean>(leftVal != rightVal);

        return newError("unknown operator: INTEGER " + op + " INTEGER");
      }
    }

    // 真偽値の演算を追加
    if (auto leftBool = std::dynamic_pointer_cast<Boolean>(left))
    {
      if (auto rightBool = std::dynamic_pointer_cast<Boolean>(right))
      {
        if (op == "==") return std::make_shared<Boolean>(leftBool->value() == rightBool->value());
        if (op == "!=") return std::make_shared<Boolean>(leftBool->value() != rightBool->value());
        return newError("unknown operator: BOOLEAN " + op + " BOOLEAN");
      }
    }

    // 文字列の演算
    if (auto leftStr = std::dynamic_pointer_cast<String>(left))
    {
      if (auto rightStr = std::dynamic_pointer_cast<String>(right))
      {
        if (op == "+")
        {
          return std::make_shared<String>(leftStr->value() + rightStr->value());
        }
        return newError("unknown operator: STRING " + op + " STRING");
      }
    }

    return newError("unknown operator: " + objectTypeToString(left->type()) + " " + 
                   op + " " + objectTypeToString(right->type()));
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

  std::string Evaluator::objectTypeToString(ObjectType type)
  {
    switch (type)
    {
    case ObjectType::INTEGER:
      return "INTEGER";
    case ObjectType::BOOLEAN:
      return "BOOLEAN";
    case ObjectType::STRING:
      return "STRING";
    case ObjectType::NULL_OBJ:
      return "NULL";
    case ObjectType::ERROR:
      return "ERROR";
    default:
      return "UNKNOWN";
    }
  }

  ObjectPtr Evaluator::newError(const std::string &message)
  {
    return std::make_shared<Error>(message);
  }

} // namespace monkey
