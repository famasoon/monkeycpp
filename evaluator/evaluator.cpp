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
        return std::make_shared<Error>(
            "wrong number of arguments. got=" + std::to_string(args.size()) + ", want=1");
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
        return std::make_shared<Error>(
            "wrong number of arguments. got=" + std::to_string(args.size()) + ", want=1");
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
        return std::make_shared<Error>(
            "wrong number of arguments. got=" + std::to_string(args.size()) + ", want=1");
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
        return std::make_shared<Error>(
            "wrong number of arguments. got=" + std::to_string(args.size()) + ", want=1");
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
        return std::make_shared<Error>(
            "wrong number of arguments. got=" + std::to_string(args.size()) + ", want=2");
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
} // namespace

ObjectPtr Evaluator::eval(const AST::Node* node)
{
    try {
        std::cout << "\n=== Starting Evaluation ===" << std::endl;
        std::cout << "Debug: Node type: " << typeid(*node).name() << std::endl;
        std::cout << "Debug: Node string representation: " << node->String() << std::endl;
        
        if (!node) {
            std::cout << "Debug: Node is null, returning Null object" << std::endl;
            return std::make_shared<Null>();
        }

        // プログラムの評価
        if (auto program = dynamic_cast<const AST::Program*>(node))
        {
            std::cout << "\nDebug: Found Program node with " << program->statements.size() << " statements" << std::endl;
            auto result = evalProgram(program);
            std::cout << "Debug: Program evaluation result: " << (result ? result->inspect() : "null") << std::endl;
            return result;
        }

        // 式文の評価
        if (auto exprStmt = dynamic_cast<const AST::ExpressionStatement*>(node))
        {
            std::cout << "\nDebug: Found ExpressionStatement" << std::endl;
            std::cout << "Debug: Expression: " << exprStmt->String() << std::endl;
            auto result = eval(exprStmt->expression.get());
            std::cout << "Debug: ExpressionStatement result: " << (result ? result->inspect() : "null") << std::endl;
            std::cout << "Debug: Result type: " << (result ? objectTypeToString(result->type()) : "null") << std::endl;
            return result;
        }

        // リテラルの評価
        if (auto intLiteral = dynamic_cast<const AST::IntegerLiteral*>(node))
        {
            std::cout << "Debug: Found IntegerLiteral: " << intLiteral->value << std::endl;
            return evalIntegerLiteral(intLiteral);
        }
        if (auto boolLiteral = dynamic_cast<const AST::BooleanLiteral*>(node))
        {
            std::cout << "Debug: Found BooleanLiteral: " << boolLiteral->value << std::endl;
            auto result = std::make_shared<Boolean>(boolLiteral->value);
            std::cout << "Debug: Created Boolean object: " << result->inspect() << std::endl;
            return result;
        }
        if (auto strLiteral = dynamic_cast<const AST::StringLiteral*>(node))
        {
            std::cout << "Debug: Found StringLiteral: " << strLiteral->getValue() << std::endl;
            return std::make_shared<String>(strLiteral->getValue());
        }

        // 演算子の評価
        if (auto prefixExpr = dynamic_cast<const AST::PrefixExpression*>(node))
        {
            std::cout << "Debug: Found PrefixExpression: " << prefixExpr->op << std::endl;
            auto right = eval(prefixExpr->right.get());
            std::cout << "Debug: PrefixExpression right operand: " 
                      << (right ? right->inspect() : "null") << std::endl;
            if (!right || isError(right)) return right;
            auto result = evalPrefixExpression(prefixExpr->op, right);
            std::cout << "Debug: PrefixExpression result: " 
                      << (result ? result->inspect() : "null") << std::endl;
            return result;
        }

        if (auto infixExpr = dynamic_cast<const AST::InfixExpression*>(node))
        {
            std::cout << "\n=== Evaluating Infix Expression ===" << std::endl;
            std::cout << "Debug: Operator: " << infixExpr->op << std::endl;
            std::cout << "Debug: Left operand: " << (infixExpr->left ? infixExpr->left->String() : "null") << std::endl;
            std::cout << "Debug: Right operand: " << (infixExpr->right ? infixExpr->right->String() : "null") << std::endl;
            
            // 文字列連結の特別処理
            if (auto leftStr = dynamic_cast<const AST::StringLiteral*>(infixExpr->left.get()))
            {
                if (auto rightStr = dynamic_cast<const AST::StringLiteral*>(infixExpr->right.get()))
                {
                    if (infixExpr->op == "+")
                    {
                        std::cout << "Debug: String concatenation" << std::endl;
                        auto result = std::make_shared<String>(leftStr->getValue() + rightStr->getValue());
                        std::cout << "Debug: Concatenation result: " << result->inspect() << std::endl;
                        return result;
                    }
                }
            }

            auto left = eval(infixExpr->left.get());
            std::cout << "Debug: Evaluated left operand: " << (left ? left->inspect() : "null") << std::endl;
            if (!left || isError(left)) return left;
            
            auto right = eval(infixExpr->right.get());
            std::cout << "Debug: Evaluated right operand: " << (right ? right->inspect() : "null") << std::endl;
            if (!right || isError(right)) return right;
            
            auto result = evalInfixExpression(infixExpr->op, left, right);
            std::cout << "Debug: InfixExpression result: " << (result ? result->inspect() : "null") << std::endl;
            return result;
        }

        // 配列リテラルの評価
        if (auto arrayLiteral = dynamic_cast<const AST::ArrayLiteral*>(node))
        {
            std::cout << "Debug: Found ArrayLiteral" << std::endl;
            return evalArrayLiteral(arrayLiteral);
        }

        // インデックス式の評価
        if (auto indexExpr = dynamic_cast<const AST::IndexExpression*>(node))
        {
            std::cout << "Debug: Found IndexExpression" << std::endl;
            return evalIndexExpression(indexExpr);
        }

        // if式の評価
        if (auto ifExpr = dynamic_cast<const AST::IfExpression*>(node))
        {
            std::cout << "Debug: Found IfExpression" << std::endl;
            auto condition = eval(ifExpr->getCondition());
            if (isError(condition)) return condition;

            if (isTruthy(condition))
            {
                std::cout << "Debug: Condition is truthy, evaluating consequence" << std::endl;
                return eval(ifExpr->getConsequence());
            }
            else if (ifExpr->getAlternative())
            {
                std::cout << "Debug: Condition is falsy, evaluating alternative" << std::endl;
                return eval(ifExpr->getAlternative());
            }
            else
            {
                std::cout << "Debug: No alternative, returning Null" << std::endl;
                return std::make_shared<Null>();
            }
        }

        // let文の評価
        if (auto letStmt = dynamic_cast<const AST::LetStatement*>(node))
        {
            std::cout << "Debug: Found LetStatement" << std::endl;
            if (!letStmt->name || !letStmt->value)
            {
                std::cout << "Debug: Invalid let statement" << std::endl;
                return newError("invalid let statement");
            }

            auto value = eval(letStmt->value.get());
            if (isError(value))
            {
                std::cout << "Debug: Error evaluating let value" << std::endl;
                return value;
            }

            env->Set(letStmt->name->value, value);
            return value;
        }

        // 識別子の評価
        if (auto ident = dynamic_cast<const AST::Identifier*>(node))
        {
            std::cout << "Debug: Found Identifier: " << ident->value << std::endl;
            return evalIdentifier(ident);
        }

        // return文の評価
        if (auto returnStmt = dynamic_cast<const AST::ReturnStatement*>(node))
        {
            std::cout << "Debug: Found ReturnStatement" << std::endl;
            if (!returnStmt->returnValue)
            {
                std::cout << "Debug: Return value is null" << std::endl;
                return newError("return value is null");
            }

            auto value = eval(returnStmt->returnValue.get());
            if (isError(value))
            {
                std::cout << "Debug: Error evaluating return value" << std::endl;
                return value;
            }

            std::cout << "Debug: Creating ReturnValue object with value: " 
                      << value->inspect() << std::endl;
            return std::make_shared<ReturnValue>(value);
        }

        // ブロック文の評価
        if (auto blockStmt = dynamic_cast<const AST::BlockStatement*>(node))
        {
            std::cout << "\n=== Evaluating Block Statement ===" << std::endl;
            std::cout << "Debug: Block contents: " << blockStmt->String() << std::endl;
            std::cout << "Debug: Number of statements: " << blockStmt->statements.size() << std::endl;
            return evalBlockStatement(blockStmt);
        }

        // while式の評価
        if (auto whileExpr = dynamic_cast<const AST::WhileExpression*>(node))
        {
            std::cout << "\n=== Evaluating While Expression ===" << std::endl;
            std::cout << "Debug: Condition: " << whileExpr->condition->String() << std::endl;
            return evalWhileExpression(whileExpr);
        }

        // for式の評価
        if (auto forExpr = dynamic_cast<const AST::ForExpression*>(node))
        {
            std::cout << "\n=== Evaluating For Expression ===" << std::endl;
            return evalForExpression(forExpr);
        }

        std::cout << "\nDebug: No matching evaluation case found" << std::endl;
        std::cout << "Debug: Node string: " << node->String() << std::endl;
        return std::make_shared<Null>();

    } catch (const std::exception& e) {
        std::cout << "\nDebug: Exception caught during evaluation" << std::endl;
        std::cout << "Debug: Exception message: " << e.what() << std::endl;
        std::cout << "Debug: Node string: " << node->String() << std::endl;
        return newError("Runtime error: " + std::string(e.what()));
    }
}

ObjectPtr Evaluator::evalPrefixExpression(const std::string& op, ObjectPtr right)
{
    std::cout << "\n=== Evaluating Prefix Expression ===" << std::endl;
    std::cout << "Debug: Operator: " << op << std::endl;
    std::cout << "Debug: Right operand: " << (right ? right->inspect() : "null") << std::endl;
    std::cout << "Debug: Right operand type: " << (right ? objectTypeToString(right->type()) : "null") << std::endl;

    if (op == "!")
    {
        return evalBangOperatorExpression(right);
    }
    else if (op == "-")
    {
        if (auto intObj = std::dynamic_pointer_cast<Integer>(right))
        {
            auto result = std::make_shared<Integer>(-intObj->value());
            std::cout << "Debug: Negation result: " << result->inspect() << std::endl;
            return result;
        }
        auto error = newError("unknown operator: -" + objectTypeToString(right->type()));
        std::cout << "Debug: Error: " << error->inspect() << std::endl;
        return error;
    }

    auto error = newError("unknown operator: " + op + objectTypeToString(right->type()));
    std::cout << "Debug: Error: " << error->inspect() << std::endl;
    return error;
}

ObjectPtr Evaluator::evalInfixExpression(const std::string& op, ObjectPtr left, ObjectPtr right)
{
    std::cout << "\n=== Evaluating Infix Expression ===" << std::endl;
    std::cout << "Debug: Operator: " << op << std::endl;
    std::cout << "Debug: Left operand: " << (left ? left->inspect() : "null") << " (type: " 
              << (left ? objectTypeToString(left->type()) : "null") << ")" << std::endl;
    std::cout << "Debug: Right operand: " << (right ? right->inspect() : "null") << " (type: "
              << (right ? objectTypeToString(right->type()) : "null") << ")" << std::endl;

    // 型が異なる場合は先にチェック
    if (left->type() != right->type())
    {
        auto error = newError("type mismatch: " + objectTypeToString(left->type()) + " " + op + " " + 
                            objectTypeToString(right->type()));
        std::cout << "Debug: Type mismatch error: " << error->inspect() << std::endl;
        return error;
    }

    // 真偽値の演算
    if (left->type() == ObjectType::BOOLEAN)
    {
        auto leftBool = std::dynamic_pointer_cast<Boolean>(left);
        auto rightBool = std::dynamic_pointer_cast<Boolean>(right);

        if (op == "==") return std::make_shared<Boolean>(leftBool->value() == rightBool->value());
        if (op == "!=") return std::make_shared<Boolean>(leftBool->value() != rightBool->value());
        if (op == "&&") return std::make_shared<Boolean>(leftBool->value() && rightBool->value());
        if (op == "||") return std::make_shared<Boolean>(leftBool->value() || rightBool->value());

        // 真偽値に対する無効な演算子の場合はエラーを返す
        auto error = newError("unknown operator: " + objectTypeToString(left->type()) + " " + op + " " + 
                            objectTypeToString(right->type()));
        std::cout << "Debug: Invalid boolean operation error: " << error->inspect() << std::endl;
        return error;
    }

    // 整数の演算
    if (left->type() == ObjectType::INTEGER)
    {
        auto leftInt = std::dynamic_pointer_cast<Integer>(left);
        auto rightInt = std::dynamic_pointer_cast<Integer>(right);

        if (op == "+") return std::make_shared<Integer>(leftInt->value() + rightInt->value());
        if (op == "-") return std::make_shared<Integer>(leftInt->value() - rightInt->value());
        if (op == "*") return std::make_shared<Integer>(leftInt->value() * rightInt->value());
        if (op == "/") {
            if (rightInt->value() == 0) return newError("division by zero");
            return std::make_shared<Integer>(leftInt->value() / rightInt->value());
        }
        if (op == "<") return std::make_shared<Boolean>(leftInt->value() < rightInt->value());
        if (op == ">") return std::make_shared<Boolean>(leftInt->value() > rightInt->value());
        if (op == "==") return std::make_shared<Boolean>(leftInt->value() == rightInt->value());
        if (op == "!=") return std::make_shared<Boolean>(leftInt->value() != rightInt->value());

        return newError("unknown operator: " + objectTypeToString(left->type()) + " " + op + " " + 
                       objectTypeToString(right->type()));
    }

    // 文字列の連結
    if (left->type() == ObjectType::STRING)
    {
        if (op == "+") {
            auto leftStr = std::dynamic_pointer_cast<String>(left);
            auto rightStr = std::dynamic_pointer_cast<String>(right);
            return std::make_shared<String>(leftStr->getValue() + rightStr->getValue());
        }
        return newError("unknown operator: " + objectTypeToString(left->type()) + " " + op + " " + 
                       objectTypeToString(right->type()));
    }

    auto result = newError("unknown operator: " + objectTypeToString(left->type()) + " " + op + " " + 
                           objectTypeToString(right->type()));
    std::cout << "Debug: Error result: " << result->inspect() << std::endl;
    return result;
}

ObjectPtr Evaluator::evalIntegerInfixExpression(
    const std::string& op, std::shared_ptr<Integer> left, std::shared_ptr<Integer> right)
{
    try {
        if (op == "+") return std::make_shared<Integer>(left->value() + right->value());
        if (op == "-") return std::make_shared<Integer>(left->value() - right->value());
        if (op == "*") return std::make_shared<Integer>(left->value() * right->value());
        if (op == "/") {
            if (right->value() == 0) {
                return newError("division by zero");
            }
            return std::make_shared<Integer>(left->value() / right->value());
        }
        // ... 他の演算子 ...
    } catch (const std::exception& e) {
        return newError("Integer operation error: " + std::string(e.what()));
    }

    return newError(
        "unknown operator: " + objectTypeToString(left->type()) + " " + op + " " + 
        objectTypeToString(right->type()));
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

ObjectPtr Evaluator::evalFunctionLiteral(const AST::FunctionLiteral *node)
{
    std::cout << "Debug: Entering evalFunctionLiteral" << std::endl;

    if (!node)
    {
        std::cout << "Debug: Node is null" << std::endl;
        return std::make_shared<Null>();
    }

    if (!node->body)
    {
        std::cout << "Debug: Function body is null" << std::endl;
        return newError("function body is null");
    }

    std::vector<std::string> params;
    params.reserve(node->parameters.size());
    for (const auto &param : node->parameters)
    {
        if (param)
        {
            params.push_back(param->value);
        }
    }

    std::cout << "Debug: Creating function object with " << params.size() << " parameters"
              << std::endl;
    std::cout << "Debug: Function body statements: " << node->body->statements.size() << std::endl;

    // 関数本体のコピーを作成
    auto bodyClone = new AST::BlockStatement(*node->body);
    std::cout << "Debug: Created body clone with " << bodyClone->statements.size() << " statements"
              << std::endl;

    // 現在の環境をキャプチャ
    auto capturedEnv = Environment::NewEnclosedEnvironment(env);

    // 関数オブジェクトを作成
    auto fn = std::make_shared<Function>(std::move(params),
                                         bodyClone, // コピーした関数本体を渡す
                                         capturedEnv);

    // bodyの有効性を確認
    if (!fn->body || fn->body->statements.empty())
    {
        std::cout << "Debug: Created function has invalid body" << std::endl;
        delete bodyClone; // エラーの場合はメモリを解放
        return newError("function creation failed");
    }

    return fn;
}

ObjectPtr Evaluator::evalIdentifier(const AST::Identifier* ident)
{
    std::cout << "Debug: Evaluating identifier: " << ident->value << std::endl;
    
    if (!env)
    {
        std::cout << "Debug: Environment is null" << std::endl;
        return newError("environment is null");
    }

    auto value = env->Get(ident->value);
    if (auto error = std::dynamic_pointer_cast<Error>(value))
    {
        std::cout << "Debug: Identifier not found: " << ident->value << std::endl;
        return error;
    }

    std::cout << "Debug: Identifier value: " << value->inspect() << std::endl;
    return value;
}

ObjectPtr Evaluator::evalBlockStatement(const AST::BlockStatement* block)
{
    std::cout << "\n=== Evaluating Block Statement ===" << std::endl;
    std::cout << "Debug: Block contents: " << block->String() << std::endl;
    std::cout << "Debug: Number of statements: " << block->statements.size() << std::endl;
    
    if (!block)
    {
        std::cout << "Debug: Block is null" << std::endl;
        return newError("block statement is null");
    }

    ObjectPtr result;
    for (const auto& stmt : block->statements)
    {
        if (!stmt)
        {
            std::cout << "Debug: Statement is null" << std::endl;
            continue;
        }

        std::cout << "Debug: Evaluating statement in block: " << stmt->String() << std::endl;
        result = eval(stmt.get());

        // エラーまたは戻り値の場合は即座に返す
        if (result)
        {
            if (isError(result))
            {
                std::cout << "Debug: Error in block statement: " << result->inspect() << std::endl;
                return result;
            }
            if (result->type() == ObjectType::RETURN_VALUE)
            {
                std::cout << "Debug: Found return value in block" << std::endl;
                return result;
            }
        }
    }

    // 結果がnullの場合はエラーを返す
    if (!result)
    {
        std::cout << "Debug: Block evaluation result is null, returning error" << std::endl;
        return newError("block statement evaluation failed");
    }

    std::cout << "Debug: Block evaluation result: " << result->inspect() << std::endl;
    return result;
}

ObjectPtr Evaluator::evalLetStatement(const AST::LetStatement* letStmt)
{
    std::cout << "Debug: Evaluating let statement" << std::endl;
    
    if (!letStmt || !letStmt->value)
    {
        std::cout << "Debug: Invalid let statement" << std::endl;
        return std::make_shared<Null>();
    }

    auto value = eval(letStmt->value.get());
    if (isError(value)) return value;

    if (!env)
    {
        std::cout << "Debug: Environment is null" << std::endl;
        return newError("environment is null");
    }

    env->Set(letStmt->name->value, value);
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

    // 引数評価
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

ObjectPtr Evaluator::evalArrayIndexExpression(const ObjectPtr& array, const ObjectPtr& index)
{
    std::cout << "Debug: Evaluating array index expression" << std::endl;

    auto arrayObj = std::dynamic_pointer_cast<Array>(array);
    if (!arrayObj)
    {
        std::cout << "Debug: Not an array object" << std::endl;
        return newError("index operator not supported: " + objectTypeToString(array->type()));
    }

    auto intIndex = std::dynamic_pointer_cast<Integer>(index);
    if (!intIndex)
    {
        std::cout << "Debug: Index is not an integer" << std::endl;
        return newError("array index must be an integer");
    }

    auto idx = intIndex->value();
    if (idx < 0 || static_cast<size_t>(idx) >= arrayObj->elements.size())
    {
        std::cout << "Debug: Index out of bounds: " << idx << std::endl;
        return std::make_shared<Null>();
    }

    std::cout << "Debug: Returning array element at index " << idx << std::endl;
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

EnvPtr Evaluator::getEnv() const
{
    return env;
}

void Evaluator::setEnv(EnvPtr newEnv)
{
    env = std::move(newEnv);
}

ObjectPtr Evaluator::evalIntegerLiteral(const AST::IntegerLiteral* node)
{
    if (!node) return std::make_shared<Null>();
    return std::make_shared<Integer>(node->value);
}

bool Evaluator::isError(const ObjectPtr& obj)
{
    if (!obj) return false;
    return obj->type() == ObjectType::ERROR;
}

ObjectPtr Evaluator::evalProgram(const AST::Program* program)
{
    if (!program)
        return std::make_shared<Null>();

    ObjectPtr result = std::make_shared<Null>();
    for (const auto& stmt : program->statements)
    {
        if (!stmt)
            continue;

        result = eval(stmt.get());

        // エラーまたは戻り値の場合は即座に返す
        if (auto returnValue = std::dynamic_pointer_cast<ReturnValue>(result))
        {
            return returnValue->value;
        }
        if (isError(result))
        {
            return result;
        }
    }

    return result;
}

bool Evaluator::isTruthy(const ObjectPtr& obj)
{
    std::cout << "Debug: Checking truthiness of object: " 
              << (obj ? obj->inspect() : "null") << std::endl;

    if (!obj)
    {
        std::cout << "Debug: Object is null, returning false" << std::endl;
        return false;
    }

    if (auto boolean = std::dynamic_pointer_cast<Boolean>(obj))
    {
        std::cout << "Debug: Object is Boolean with value: " << boolean->value() << std::endl;
        return boolean->value();
    }

    if (obj->type() == ObjectType::NULL_OBJ)
    {
        std::cout << "Debug: Object is Null, returning false" << std::endl;
        return false;
    }

    if (auto integer = std::dynamic_pointer_cast<Integer>(obj))
    {
        std::cout << "Debug: Object is Integer with value: " << integer->value() << std::endl;
        return integer->value() != 0;
    }

    std::cout << "Debug: Object is truthy by default" << std::endl;
    return true;
}

ObjectPtr Evaluator::evalWhileExpression(const AST::WhileExpression* whileExpr) {
    std::cout << "\n=== Evaluating While Expression ===" << std::endl;
    
    if (!whileExpr->condition || !whileExpr->body) {
        std::cout << "Debug: Invalid while expression" << std::endl;
        return newError("invalid while expression");
    }

    ObjectPtr result = std::make_shared<Null>();
    while (true) {
        auto condition = eval(whileExpr->condition.get());
        if (isError(condition)) return condition;

        if (!isTruthy(condition)) {
            break;
        }

        result = eval(whileExpr->body.get());
        if (isError(result)) return result;

        // return文が見つかった場合は即座に返す
        if (result && result->type() == ObjectType::RETURN_VALUE) {
            return result;
        }
    }

    return result;
}

ObjectPtr Evaluator::evalForExpression(const AST::ForExpression* forExpr) {
    std::cout << "\n=== Evaluating For Expression ===" << std::endl;
    
    if (!forExpr->init || !forExpr->condition || !forExpr->update || !forExpr->body) {
        std::cout << "Debug: Invalid for expression" << std::endl;
        return newError("invalid for expression");
    }

    // 初期化式を評価
    auto init = eval(forExpr->init.get());
    if (isError(init)) return init;

    ObjectPtr result = std::make_shared<Null>();
    while (true) {
        // 条件式を評価
        auto condition = eval(forExpr->condition.get());
        if (isError(condition)) return condition;

        if (!isTruthy(condition)) {
            break;
        }

        // 本体を評価
        result = eval(forExpr->body.get());
        if (isError(result)) return result;

        // return文が見つかった場合は即座に返す
        if (result && result->type() == ObjectType::RETURN_VALUE) {
            return result;
        }

        // 更新式を評価
        auto update = eval(forExpr->update.get());
        if (isError(update)) return update;
    }

    return result;
}

} // namespace monkey
