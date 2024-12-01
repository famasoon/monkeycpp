#include "evaluator.hpp"
#include <iostream>

namespace monkey
{

  namespace
  {
    // デバッグ用の定数
    constexpr bool DEBUG_OUTPUT = false;

    // デバッグ出力用のヘルパー関数
    void debugPrint(const std::string &message)
    {
      if (DEBUG_OUTPUT)
      {
        std::cout << "Debug: " << message << "\n";
      }
    }

    // エラーチェック用のヘルパー関数
    bool isError(const ObjectPtr &obj)
    {
      return obj != nullptr && obj->type() == ObjectType::ERROR;
    }
  }

  ObjectPtr Evaluator::eval(const AST::Node *node)
  {
    if (!node)
      return std::make_shared<Null>();

    // リテラルの評価
    if (auto *intLiteral = dynamic_cast<const AST::IntegerLiteral *>(node))
    {
      return evalIntegerLiteral(intLiteral);
    }
    if (auto *boolLiteral = dynamic_cast<const AST::BooleanLiteral *>(node))
    {
      return evalBooleanLiteral(boolLiteral);
    }
    if (auto *strLiteral = dynamic_cast<const AST::StringLiteral *>(node))
    {
      return evalStringLiteral(strLiteral);
    }
    if (auto *funcLiteral = dynamic_cast<const AST::FunctionLiteral *>(node))
    {
      return evalFunctionLiteral(funcLiteral);
    }
    if (auto *identifier = dynamic_cast<const AST::Identifier *>(node))
    {
      return evalIdentifier(identifier);
    }

    // 文の評価
    if (auto *program = dynamic_cast<const AST::Program *>(node))
    {
      return evalProgram(program);
    }
    if (auto *blockStmt = dynamic_cast<const AST::BlockStatement *>(node))
    {
      return evalBlockStatement(blockStmt);
    }
    if (auto *letStmt = dynamic_cast<const AST::LetStatement *>(node))
    {
      return evalLetStatement(letStmt);
    }
    if (auto *returnStmt = dynamic_cast<const AST::ReturnStatement *>(node))
    {
      return evalReturnStatement(returnStmt);
    }
    if (auto *exprStmt = dynamic_cast<const AST::ExpressionStatement *>(node))
    {
      return evalExpressionStatement(exprStmt);
    }

    // 式の評価
    if (auto *prefixExpr = dynamic_cast<const AST::PrefixExpression *>(node))
    {
      return evalPrefixExpression(prefixExpr);
    }
    if (auto *infixExpr = dynamic_cast<const AST::InfixExpression *>(node))
    {
      return evalInfixExpression(infixExpr);
    }
    if (auto *callExpr = dynamic_cast<const AST::CallExpression *>(node))
    {
      return evalCallExpression(callExpr);
    }

    return std::make_shared<Null>();
  }

  ObjectPtr Evaluator::evalProgram(const AST::Program *program)
  {
    if (!program)
      return std::make_shared<Null>();

    ObjectPtr result;
    for (const std::unique_ptr<AST::Statement> &stmt : program->statements)
    {
      if (!stmt)
        continue;

      result = eval(stmt.get());
      if (isError(result))
        return result;
    }
    return result ? result : std::make_shared<Null>();
  }

  ObjectPtr Evaluator::evalPrefixExpression(const AST::PrefixExpression *expr)
  {
    if (!expr || !expr->right)
    {
      return newError("invalid prefix expression");
    }

    auto right = eval(expr->right.get());
    if (isError(right))
      return right;

    if (expr->op == "!")
    {
      return evalBangOperatorExpression(right);
    }
    if (expr->op == "-")
    {
      return evalMinusPrefixOperatorExpression(right);
    }

    return newError("unknown operator: " + expr->op);
  }

  ObjectPtr Evaluator::evalMinusPrefixOperatorExpression(const ObjectPtr &right)
  {
    if (auto integer = std::dynamic_pointer_cast<Integer>(right))
    {
      return std::make_shared<Integer>(-integer->value());
    }
    return newError("invalid operator: -" + objectTypeToString(right->type()));
  }

  ObjectPtr Evaluator::evalInfixExpression(const AST::InfixExpression *expr)
  {
    if (!expr || !expr->left || !expr->right)
    {
      return newError("invalid infix expression");
    }

    auto left = eval(expr->left.get());
    if (isError(left))
      return left;

    auto right = eval(expr->right.get());
    if (isError(right))
      return right;

    return evalInfixOperation(expr->op, left, right);
  }

  ObjectPtr Evaluator::evalInfixOperation(const std::string &op, const ObjectPtr &left, const ObjectPtr &right)
  {
    if (left->type() != right->type())
    {
      return newError("type mismatch: " + objectTypeToString(left->type()) + " " +
                      op + " " + objectTypeToString(right->type()));
    }

    // 整数演算
    std::shared_ptr<Integer> leftInt = std::dynamic_pointer_cast<Integer>(left);
    if (leftInt)
    {
      return evalIntegerInfixExpression(op, leftInt, std::dynamic_pointer_cast<Integer>(right));
    }

    // 真偽値演算
    std::shared_ptr<Boolean> leftBool = std::dynamic_pointer_cast<Boolean>(left);
    if (leftBool)
    {
      return evalBooleanInfixExpression(op, leftBool, std::dynamic_pointer_cast<Boolean>(right));
    }

    // 文字列演算
    std::shared_ptr<String> leftStr = std::dynamic_pointer_cast<String>(left);
    if (leftStr)
    {
      return evalStringInfixExpression(op, leftStr, std::dynamic_pointer_cast<String>(right));
    }

    return newError("unknown operator: " + objectTypeToString(left->type()) + " " +
                    op + " " + objectTypeToString(right->type()));
  }

  ObjectPtr Evaluator::evalIntegerInfixExpression(
      const std::string &op,
      const std::shared_ptr<Integer> &left,
      const std::shared_ptr<Integer> &right)
  {
    auto leftVal = left->value();
    auto rightVal = right->value();

    if (op == "*")
      return std::make_shared<Integer>(leftVal * rightVal);
    if (op == "/")
    {
      if (rightVal == 0)
        return newError("division by zero");
      return std::make_shared<Integer>(leftVal / rightVal);
    }
    if (op == "+")
      return std::make_shared<Integer>(leftVal + rightVal);
    if (op == "-")
      return std::make_shared<Integer>(leftVal - rightVal);
    if (op == "<")
      return std::make_shared<Boolean>(leftVal < rightVal);
    if (op == ">")
      return std::make_shared<Boolean>(leftVal > rightVal);
    if (op == "==")
      return std::make_shared<Boolean>(leftVal == rightVal);
    if (op == "!=")
      return std::make_shared<Boolean>(leftVal != rightVal);

    return newError("unknown operator: INTEGER " + op + " INTEGER");
  }

  ObjectPtr Evaluator::evalBooleanInfixExpression(
      const std::string &op,
      const std::shared_ptr<Boolean> &left,
      const std::shared_ptr<Boolean> &right)
  {
    if (op == "==")
      return std::make_shared<Boolean>(left->value() == right->value());
    if (op == "!=")
      return std::make_shared<Boolean>(left->value() != right->value());
    return newError("unknown operator: BOOLEAN " + op + " BOOLEAN");
  }

  ObjectPtr Evaluator::evalStringInfixExpression(
      const std::string &op,
      const std::shared_ptr<String> &left,
      const std::shared_ptr<String> &right)
  {
    if (op == "+")
      return std::make_shared<String>(left->value() + right->value());
    return newError("unknown operator: STRING " + op + " STRING");
  }

  ObjectPtr Evaluator::evalIntegerLiteral(const AST::IntegerLiteral *node)
  {
    if (!node)
    {
      return std::make_shared<Null>();
    }
    return std::static_pointer_cast<Object>(std::make_shared<Integer>(node->value));
  }

  ObjectPtr Evaluator::evalBooleanLiteral(const AST::BooleanLiteral *node)
  {
    if (!node)
    {
      return std::make_shared<Null>();
    }
    debugPrint("Evaluating boolean literal: " + std::string(node->value ? "true" : "false"));
    return std::static_pointer_cast<Object>(std::make_shared<Boolean>(node->value));
  }

  ObjectPtr Evaluator::evalStringLiteral(const AST::StringLiteral *node)
  {
    if (!node)
    {
      return std::make_shared<Null>();
    }
    debugPrint("Evaluating string literal: \"" + node->value + "\"");
    return std::static_pointer_cast<Object>(std::make_shared<String>(node->value));
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
    debugPrint("Error: " + message);
    return std::make_shared<Error>(message);
  }

  ObjectPtr Evaluator::evalFunctionLiteral(const AST::FunctionLiteral *node)
  {
    if (!node)
    {
      return std::make_shared<Null>();
    }

    std::vector<std::string> params;
    for (const auto &param : node->parameters)
    {
      if (param)
      {
        params.push_back(param->value);
      }
    }

    return std::make_shared<Function>(params, node->body.get(), nullptr);
  }

  ObjectPtr Evaluator::evalIdentifier(const AST::Identifier *node)
  {
    if (!node)
    {
      return std::make_shared<Null>();
    }
    return std::make_shared<String>(node->value); // 簡単な実装として文字列として評価
  }

  ObjectPtr Evaluator::evalBlockStatement(const AST::BlockStatement *block)
  {
    if (!block)
    {
      return std::make_shared<Null>();
    }

    ObjectPtr result;
    for (const auto &stmt : block->statements)
    {
      if (!stmt)
        continue;
      
      result = eval(stmt.get());
      if (isError(result))
      {
        return result;
      }
    }
    return result ? result : std::make_shared<Null>();
  }

  ObjectPtr Evaluator::evalLetStatement(const AST::LetStatement *letStmt)
  {
    if (!letStmt || !letStmt->value)
    {
      return std::make_shared<Null>();
    }

    auto value = eval(letStmt->value.get());
    if (isError(value))
    {
      return value;
    }

    return value;
  }

  ObjectPtr Evaluator::evalReturnStatement(const AST::ReturnStatement *returnStmt)
  {
    if (!returnStmt || !returnStmt->returnValue)
    {
      return std::make_shared<Null>();
    }

    auto value = eval(returnStmt->returnValue.get());
    if (isError(value))
    {
      return value;
    }

    return std::make_shared<ReturnValue>(value);
  }

  ObjectPtr Evaluator::evalCallExpression(const AST::CallExpression *call)
  {
    if (!call || !call->function)
    {
      return newError("invalid call expression");
    }

    auto function = eval(call->function.get());
    if (isError(function))
    {
      return function;
    }

    std::vector<ObjectPtr> args;
    for (const auto &arg : call->arguments)
    {
      if (!arg)
        continue;
      
      auto evaluated = eval(arg.get());
      if (isError(evaluated))
      {
        return evaluated;
      }
      args.push_back(evaluated);
    }

    if (auto fn = std::dynamic_pointer_cast<Function>(function))
    {
      // 関数呼び出しの実装（環境の設定なども必要）
      return std::make_shared<Null>(); // 仮の実装
    }

    return newError("not a function: " + objectTypeToString(function->type()));
  }

  ObjectPtr Evaluator::evalExpressionStatement(const AST::ExpressionStatement* exprStmt)
  {
    if (!exprStmt || !exprStmt->expression)
    {
        return std::make_shared<Null>();
    }
    return eval(exprStmt->expression.get());
  }

} // namespace monkey
