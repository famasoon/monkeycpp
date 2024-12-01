#include "evaluator.hpp"
#include <iostream>

namespace monkey
{

  namespace
  {
    // デバッグ用の定数
    constexpr bool DEBUG_OUTPUT = false;
    constexpr size_t GC_THRESHOLD = 1000; // ガベージコレクションのしきい値
    size_t allocatedObjects = 0;          // 割り当てられたオブジェクトの数

    // objectTypeToString関数の前方宣言
    std::string objectTypeToString(ObjectType type);

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

    // オブジェクトの割り当てを追跡
    void trackObject()
    {
      allocatedObjects++;
      debugPrint("Allocated objects: " + std::to_string(allocatedObjects));
    }

    // 配列用の組み込み関数
    ObjectPtr builtinLen(const std::vector<ObjectPtr> &args)
    {
      if (args.size() != 1)
      {
        return std::make_shared<Error>("wrong number of arguments. got=" +
                                       std::to_string(args.size()) + ", want=1");
      }

      if (auto array = std::dynamic_pointer_cast<Array>(args[0]))
      {
        return std::make_shared<Integer>(array->elements.size());
      }

      return std::make_shared<Error>("argument to `len` not supported, got " +
                                     objectTypeToString(args[0]->type()));
    }

    ObjectPtr builtinFirst(const std::vector<ObjectPtr> &args)
    {
      if (args.size() != 1)
      {
        return std::make_shared<Error>("wrong number of arguments. got=" +
                                       std::to_string(args.size()) + ", want=1");
      }

      auto array = std::dynamic_pointer_cast<Array>(args[0]);
      if (!array)
      {
        return std::make_shared<Error>("argument to `first` must be ARRAY, got " +
                                       objectTypeToString(args[0]->type()));
      }

      if (array->elements.empty())
      {
        return std::make_shared<Null>();
      }

      return array->elements[0];
    }

    ObjectPtr builtinLast(const std::vector<ObjectPtr> &args)
    {
      if (args.size() != 1)
      {
        return std::make_shared<Error>("wrong number of arguments. got=" +
                                       std::to_string(args.size()) + ", want=1");
      }

      auto array = std::dynamic_pointer_cast<Array>(args[0]);
      if (!array)
      {
        return std::make_shared<Error>("argument to `last` must be ARRAY, got " +
                                       objectTypeToString(args[0]->type()));
      }

      if (array->elements.empty())
      {
        return std::make_shared<Null>();
      }

      return array->elements.back();
    }

    ObjectPtr builtinRest(const std::vector<ObjectPtr> &args)
    {
      if (args.size() != 1)
      {
        return std::make_shared<Error>("wrong number of arguments. got=" +
                                       std::to_string(args.size()) + ", want=1");
      }

      auto array = std::dynamic_pointer_cast<Array>(args[0]);
      if (!array)
      {
        return std::make_shared<Error>("argument to `rest` must be ARRAY, got " +
                                       objectTypeToString(args[0]->type()));
      }

      if (array->elements.empty())
      {
        return std::make_shared<Null>();
      }

      std::vector<ObjectPtr> newElements(array->elements.begin() + 1, array->elements.end());
      return std::make_shared<Array>(std::move(newElements));
    }

    ObjectPtr builtinPush(const std::vector<ObjectPtr> &args)
    {
      if (args.size() != 2)
      {
        return std::make_shared<Error>("wrong number of arguments. got=" +
                                       std::to_string(args.size()) + ", want=2");
      }

      auto array = std::dynamic_pointer_cast<Array>(args[0]);
      if (!array)
      {
        return std::make_shared<Error>("argument to `push` must be ARRAY, got " +
                                       objectTypeToString(args[0]->type()));
      }

      std::vector<ObjectPtr> newElements = array->elements;
      newElements.push_back(args[1]);
      return std::make_shared<Array>(std::move(newElements));
    }

    // objectTypeToString関数の実装
    std::string objectTypeToString(ObjectType type)
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
      case ObjectType::ARRAY:
        return "ARRAY";
      case ObjectType::HASH:
        return "HASH";
      case ObjectType::FUNCTION:
        return "FUNCTION";
      case ObjectType::BUILTIN:
        return "BUILTIN";
      case ObjectType::RETURN_VALUE:
        return "RETURN_VALUE";
      default:
        return "UNKNOWN";
      }
    }
  }

  ObjectPtr Evaluator::eval(const AST::Node *node)
  {
    if (!node)
      return std::make_shared<Null>();

    // オブジェクトの割り当てを追跡
    trackObject();

    // しきい値を超えた場合、ガベージコレクションを実行
    if (allocatedObjects > GC_THRESHOLD)
    {
      debugPrint("Running garbage collection...");
      collectGarbage();
      allocatedObjects = 0;
    }

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
    if (auto arrayLiteral = dynamic_cast<const AST::ArrayLiteral *>(node))
    {
      return evalArrayLiteral(arrayLiteral);
    }
    if (auto indexExpr = dynamic_cast<const AST::IndexExpression *>(node))
    {
      return evalIndexExpression(indexExpr);
    }
    if (auto hashLiteral = dynamic_cast<const AST::HashLiteral *>(node))
    {
      return evalHashLiteral(hashLiteral);
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

    // 字列演算
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
      return std::make_shared<String>(left->getValue() + right->getValue());
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
    return std::make_shared<String>(node->getValue());
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

  ObjectPtr Evaluator::newError(const std::string &message)
  {
    debugPrint("Error: " + message);
    return std::make_shared<Error>(message);
  }

  ObjectPtr Evaluator::evalFunctionLiteral(const AST::FunctionLiteral *node) {
    std::cout << "Debug: Entering evalFunctionLiteral" << std::endl;
    
    if (!node) {
        std::cout << "Debug: Node is null" << std::endl;
        return std::make_shared<Null>();
    }

    if (!node->body) {
        std::cout << "Debug: Function body is null" << std::endl;
        return newError("function body is null");
    }

    std::vector<std::string> params;
    params.reserve(node->parameters.size());
    for (const auto &param : node->parameters) {
        if (param) {
            params.push_back(param->value);
        }
    }

    std::cout << "Debug: Creating function object with " 
              << params.size() << " parameters" << std::endl;
    std::cout << "Debug: Function body statements: " << node->body->statements.size() << std::endl;

    // 関数本体のコピーを作成
    auto bodyClone = new AST::BlockStatement(*node->body);
    std::cout << "Debug: Created body clone with " << bodyClone->statements.size() << " statements" << std::endl;

    // 現在の環境をキャプチャ
    auto capturedEnv = Environment::NewEnclosedEnvironment(env);
    
    // 関数オブジェクトを作成
    auto fn = std::make_shared<Function>(
        std::move(params),
        bodyClone,  // コピーした関数本体を渡す
        capturedEnv
    );

    // bodyの有効性を確認
    if (!fn->body || fn->body->statements.empty()) {
        std::cout << "Debug: Created function has invalid body" << std::endl;
        delete bodyClone;  // エラーの場合はメモリを解放
        return newError("function creation failed");
    }

    return fn;
  }

  ObjectPtr Evaluator::evalIdentifier(const AST::Identifier *node)
  {
    if (!node)
    {
      return std::make_shared<Null>();
    }

    if (auto obj = env->Get(node->value))
    {
      return obj;
    }

    return newError("identifier not found: " + node->value);
  }

  ObjectPtr Evaluator::evalBlockStatement(const AST::BlockStatement *block) {
    if (!block) {
        std::cout << "Debug: Block is null" << std::endl;
        return std::make_shared<Null>();
    }

    std::cout << "Debug: Block address: " << block << std::endl;
    
    // statementsメンバーの有効性を確認
    try {
        size_t stmtSize = block->statements.size();
        std::cout << "Debug: Number of statements: " << stmtSize << std::endl;

        if (stmtSize > 1000) {  // より現実的な上限値に変更
            std::cout << "Debug: Too many statements" << std::endl;
            return newError("too many statements in block");
        }

        ObjectPtr result = std::make_shared<Null>();

        for (const auto &stmt : block->statements) {
            if (!stmt) {
                std::cout << "Debug: Statement is null" << std::endl;
                continue;
            }

            std::cout << "Debug: Evaluating statement: " << stmt->String() << std::endl;
            result = eval(stmt.get());
            
            if (isError(result)) {
                return result;
            }

            if (result && result->type() == ObjectType::RETURN_VALUE) {
                return result;
            }
        }

        return result;
    } catch (const std::exception& e) {
        std::cout << "Debug: Exception caught: " << e.what() << std::endl;
        return newError("invalid block statement");
    }
  }

  ObjectPtr Evaluator::evalLetStatement(const AST::LetStatement *letStmt)
  {
    if (!letStmt || !letStmt->value || !letStmt->name)
    {
      return std::make_shared<Null>();
    }

    auto value = eval(letStmt->value.get());
    if (isError(value))
    {
      return value;
    }

    // 変数をバインドする際にも追跡
    trackObject();
    return env->Set(letStmt->name->value, value);
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
    std::cout << "Debug: Entering evalCallExpression" << std::endl;

    if (!call || !call->function)
    {
      std::cout << "Debug: Invalid call expression" << std::endl;
      return newError("invalid call expression");
    }

    // 関数を評価
    std::cout << "Debug: Evaluating function" << std::endl;
    auto function = eval(call->function.get());
    if (isError(function))
    {
      std::cout << "Debug: Function evaluation error" << std::endl;
      return function;
    }

    // 引数を評価
    std::cout << "Debug: Evaluating arguments" << std::endl;
    std::vector<ObjectPtr> args;
    args.reserve(call->arguments.size());
    for (const auto &arg : call->arguments)
    {
      if (!arg)
        continue;

      auto evaluated = eval(arg.get());
      if (isError(evaluated))
      {
        std::cout << "Debug: Argument evaluation error" << std::endl;
        return evaluated;
      }
      args.push_back(evaluated);
    }

    // 関数オブジェクトの場合
    if (auto fn = std::dynamic_pointer_cast<Function>(function))
    {
      std::cout << "Debug: Found function object" << std::endl;

      if (fn->parameters.size() != args.size())
      {
        std::cout << "Debug: Wrong number of arguments" << std::endl;
        return newError("wrong number of arguments: expected " +
                        std::to_string(fn->parameters.size()) + ", got " +
                        std::to_string(args.size()));
      }

      if (!fn->body)
      {
        std::cout << "Debug: Function body is null" << std::endl;
        return newError("function body is null");
      }

      // 関数の環境を基に新しい環境を作成
      std::cout << "Debug: Creating new environment" << std::endl;
      auto newEnv = Environment::NewEnclosedEnvironment(env);

      // パラメータをバインド
      std::cout << "Debug: Binding parameters" << std::endl;
      for (size_t i = 0; i < fn->parameters.size(); i++)
      {
        newEnv->Set(fn->parameters[i], args[i]);
      }

      // 現在の環境を一時的に保存
      std::cout << "Debug: Saving current environment" << std::endl;
      auto savedEnv = env;

      // 新しい環境を設定
      std::cout << "Debug: Setting new environment" << std::endl;
      env = newEnv;

      // 関数本体を評価
      std::cout << "Debug: Evaluating function body" << std::endl;
      auto result = evalBlockStatement(fn->body);

      // 環境を元に戻す
      std::cout << "Debug: Restoring environment" << std::endl;
      env = savedEnv;

      // ReturnValueの場合は、内部の値を返す
      if (auto returnValue = std::dynamic_pointer_cast<ReturnValue>(result))
      {
        std::cout << "Debug: Returning return value" << std::endl;
        return returnValue->value;
      }

      std::cout << "Debug: Returning result" << std::endl;
      return result;
    }

    // ビルトイン関数の場合
    if (auto builtin = std::dynamic_pointer_cast<Builtin>(function))
    {
      std::cout << "Debug: Executing builtin function" << std::endl;
      return builtin->fn(args);
    }

    std::cout << "Debug: Not a function error" << std::endl;
    return newError("not a function: " + objectTypeToString(function->type()));
  }

  ObjectPtr Evaluator::evalExpressionStatement(const AST::ExpressionStatement *exprStmt)
  {
    if (!exprStmt || !exprStmt->expression)
    {
      return std::make_shared<Null>();
    }
    return eval(exprStmt->expression.get());
  }

  Evaluator::Evaluator() : env(Environment::NewEnvironment())
  {
    env->Set("len", std::make_shared<Builtin>(builtinLen));
    env->Set("first", std::make_shared<Builtin>(builtinFirst));
    env->Set("last", std::make_shared<Builtin>(builtinLast));
    env->Set("rest", std::make_shared<Builtin>(builtinRest));
    env->Set("push", std::make_shared<Builtin>(builtinPush));
  }

  void Evaluator::collectGarbage()
  {
    if (env)
    {
      env->MarkAndSweep();
    }
  }

  ObjectPtr Evaluator::evalArrayLiteral(const AST::ArrayLiteral *array)
  {
    if (!array)
    {
      return std::make_shared<Null>();
    }

    std::vector<ObjectPtr> elements;
    elements.reserve(array->elements.size());

    for (const auto &elem : array->elements)
    {
      if (!elem)
        continue;

      auto evaluated = eval(elem.get());
      if (isError(evaluated))
      {
        return evaluated;
      }
      elements.push_back(evaluated);
    }

    return std::make_shared<Array>(std::move(elements));
  }

  ObjectPtr Evaluator::evalIndexExpression(const AST::IndexExpression *indexExpr)
  {
    if (!indexExpr || !indexExpr->left || !indexExpr->index)
    {
      return newError("invalid index expression");
    }

    auto left = eval(indexExpr->left.get());
    if (isError(left))
    {
      return left;
    }

    auto index = eval(indexExpr->index.get());
    if (isError(index))
    {
      return index;
    }

    if (left->type() == ObjectType::ARRAY)
    {
      return evalArrayIndexExpression(left, index);
    }
    else if (left->type() == ObjectType::HASH)
    {
      return evalHashIndexExpression(left, index);
    }

    return newError("index operator not supported: " + objectTypeToString(left->type()));
  }

  ObjectPtr Evaluator::evalArrayIndexExpression(const ObjectPtr &array, const ObjectPtr &index)
  {
    auto arrayObj = std::dynamic_pointer_cast<Array>(array);
    if (!arrayObj)
    {
      return newError("not an array");
    }

    auto intIndex = std::dynamic_pointer_cast<Integer>(index);
    if (!intIndex)
    {
      return newError("array index must be an integer");
    }

    auto idx = intIndex->value();
    auto max = static_cast<int64_t>(arrayObj->elements.size());

    if (idx < 0 || idx >= max)
    {
      return std::make_shared<Null>();
    }

    return arrayObj->elements[idx];
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
    case ObjectType::ARRAY:
      return "ARRAY";
    case ObjectType::HASH:
      return "HASH";
    case ObjectType::FUNCTION:
      return "FUNCTION";
    case ObjectType::BUILTIN:
      return "BUILTIN";
    case ObjectType::RETURN_VALUE:
      return "RETURN_VALUE";
    default:
      return "UNKNOWN";
    }
  }

  ObjectPtr Evaluator::evalHashLiteral(const AST::HashLiteral *node)
  {
    if (!node)
    {
      return std::make_shared<Null>();
    }

    auto hash = std::make_shared<Hash>();

    for (const auto &pair : node->pairs)
    {
      auto key = eval(pair.first.get());
      if (isError(key))
        return key;

      auto hashKey = dynamic_cast<HashKey *>(key.get());
      if (!hashKey)
      {
        return newError("unusable as hash key: " + objectTypeToString(key->type()));
      }

      auto value = eval(pair.second.get());
      if (isError(value))
        return value;

      size_t hashValue = hashKey->hash();
      hash->pairs[hashValue] = HashPair(key, value);
    }

    return hash;
  }

  ObjectPtr Evaluator::evalHashIndexExpression(const ObjectPtr &hash, const ObjectPtr &index)
  {
    auto hashObj = std::dynamic_pointer_cast<Hash>(hash);
    if (!hashObj)
    {
      return newError("index operator not supported: " + objectTypeToString(hash->type()));
    }

    auto hashKey = dynamic_cast<HashKey *>(index.get());
    if (!hashKey)
    {
      return newError("unusable as hash key: " + objectTypeToString(index->type()));
    }

    auto pair = hashObj->pairs.find(hashKey->hash());
    if (pair == hashObj->pairs.end())
    {
      return std::make_shared<Null>();
    }

    return pair->second.value;
  }

  EnvPtr Evaluator::getEnv() const {
    return env;
  }

  void Evaluator::setEnv(EnvPtr newEnv) {
    env = std::move(newEnv);
  }

} // namespace monkey
