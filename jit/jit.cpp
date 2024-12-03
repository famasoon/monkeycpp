#include "jit.hpp"
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <iostream>

namespace JIT
{

Compiler::Compiler()
{
    context = std::make_unique<llvm::LLVMContext>();
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    module = std::make_unique<llvm::Module>("monkey_jit", *context);
    setOptimizationLevel(2); // デフォルトで-O2最適化
}

void Compiler::setOptimizationLevel(unsigned level)
{
    initializeOptimizations(level);
}

void Compiler::initializeOptimizations(unsigned level)
{
    // 分析マネージャーの初期化のみを行い、最適化は無効化
    moduleAnalysisManager = llvm::ModuleAnalysisManager();
}

void Compiler::compile(const AST::Program& program)
{
    std::cout << "Starting compilation..." << std::endl;
    
    // 既存のモジュールをクリア
    module = std::make_unique<llvm::Module>("monkey_jit", *context);
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    namedValues.clear();
    
    try {
        std::cout << "Creating main function..." << std::endl;
        // メイン関数の作成
        llvm::FunctionType* mainType = llvm::FunctionType::get(
            llvm::Type::getInt64Ty(*context), false);
        llvm::Function* mainFunc = llvm::Function::Create(
            mainType, llvm::Function::ExternalLinkage, "main", module.get());
        
        std::cout << "Creating entry block..." << std::endl;
        // エントリーブロックの作成
        llvm::BasicBlock* bb = llvm::BasicBlock::Create(*context, "entry", mainFunc);
        builder->SetInsertPoint(bb);
        
        // 文のコンパイル
        llvm::Value* lastValue = nullptr;
        std::cout << "Compiling statements..." << std::endl;
        for (const auto& stmt : program.statements)
        {
            if (stmt)
            {
                std::cout << "Compiling statement: " << stmt->String() << std::endl;
                lastValue = compileStatement(stmt.get());
                if (lastValue)
                {
                    std::cout << "Statement compiled successfully" << std::endl;
                }
                else
                {
                    std::cout << "Statement compilation returned nullptr" << std::endl;
                }
            }
        }
        
        std::cout << "Setting return value..." << std::endl;
        // 戻り値の設定
        if (!lastValue)
        {
            lastValue = llvm::ConstantInt::get(*context, llvm::APInt(64, 0));
        }
        builder->CreateRet(lastValue);
        
        std::cout << "Verifying module..." << std::endl;
        // 検証を実行
        std::string error;
        llvm::raw_string_ostream errorStream(error);
        if (llvm::verifyModule(*module, &errorStream))
        {
            std::cout << "Module verification failed!" << std::endl;
            throw std::runtime_error("Module verification failed: " + error);
        }
        
        // 最適化は一時的にスキップ
        std::cout << "Skipping optimizations..." << std::endl;
        std::cout << "Compilation completed successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Compilation failed with exception: " << e.what() << std::endl;
        throw;
    }
}

void Compiler::runOptimizations()
{
    // 最適化を一時的に無効化
    std::cout << "Optimizations disabled" << std::endl;
    return;
}

llvm::Value* Compiler::compileStatement(const AST::Statement* stmt)
{
    if (auto exprStmt = dynamic_cast<const AST::ExpressionStatement*>(stmt))
    {
        if (exprStmt->expression)
        {
            return compileExpression(exprStmt->expression.get());
        }
    }
    else if (auto letStmt = dynamic_cast<const AST::LetStatement*>(stmt))
    {
        compileLetStatement(letStmt);
    }
    else if (auto ifStmt = dynamic_cast<const AST::IfExpression*>(stmt))
    {
        return compileIfExpression(ifStmt);
    }
    else if (auto whileStmt = dynamic_cast<const AST::WhileExpression*>(stmt))
    {
        return compileWhileExpression(whileStmt);
    }
    return nullptr;
}

llvm::Value *Compiler::compileExpression(const AST::Expression *expr)
{
    try {
        if (auto intLiteral = dynamic_cast<const AST::IntegerLiteral*>(expr))
        {
            return compileIntegerLiteral(intLiteral);
        }
        if (auto infixExpr = dynamic_cast<const AST::InfixExpression*>(expr))
        {
            return compileInfixExpression(infixExpr);
        }
        if (auto prefixExpr = dynamic_cast<const AST::PrefixExpression*>(expr))
        {
            return compilePrefixExpression(prefixExpr);
        }
        if (auto booleanLiteral = dynamic_cast<const AST::BooleanLiteral*>(expr))
        {
            return compileBooleanLiteral(booleanLiteral);
        }
        if (auto identifier = dynamic_cast<const AST::Identifier*>(expr))
        {
            return compileIdentifier(identifier);
        }
        if (auto functionLiteral = dynamic_cast<const AST::FunctionLiteral*>(expr))
        {
            return compileFunctionLiteral(functionLiteral);
        }
        if (auto callExpr = dynamic_cast<const AST::CallExpression*>(expr))
        {
            return compileCallExpression(callExpr);
        }
    } catch (...) {
        return nullptr;
    }
    return nullptr;
}

llvm::Value *Compiler::compileIntegerLiteral(const AST::IntegerLiteral *literal)
{
    return llvm::ConstantInt::get(*context, llvm::APInt(64, literal->value, true));
}

llvm::Value *Compiler::compileInfixExpression(const AST::InfixExpression* infix)
{
    std::cout << "Compiling infix expression: " << infix->String() << std::endl;
    
    if (!infix || !infix->left || !infix->right)
    {
        std::cout << "Invalid infix expression structure" << std::endl;
        return nullptr;
    }

    std::cout << "Compiling left operand..." << std::endl;
    auto left = compileExpression(infix->left.get());
    if (!left)
    {
        std::cout << "Left operand compilation failed" << std::endl;
        return nullptr;
    }

    std::cout << "Compiling right operand..." << std::endl;
    auto right = compileExpression(infix->right.get());
    if (!right)
    {
        std::cout << "Right operand compilation failed" << std::endl;
        return nullptr;
    }

    // 両オペランドの型を64ビット整数に変換
    if (left->getType()->isIntegerTy(1))
    {
        left = builder->CreateZExt(left, llvm::Type::getInt64Ty(*context));
    }
    if (right->getType()->isIntegerTy(1))
    {
        right = builder->CreateZExt(right, llvm::Type::getInt64Ty(*context));
    }

    std::cout << "Creating operation: " << infix->op << std::endl;
    if (infix->op == "+")
    {
        auto result = builder->CreateAdd(left, right);
        result->setName("add");
        return result;
    }
    else if (infix->op == "*")
    {
        auto result = builder->CreateMul(left, right);
        result->setName("mul");
        return result;
    }
    else if (infix->op == "-")
    {
        return builder->CreateSub(left, right, "subtmp");
    }
    else if (infix->op == "/")
    {
        return builder->CreateSDiv(left, right, "divtmp");
    }

    std::cout << "Unknown operator: " << infix->op << std::endl;
    return nullptr;
}

llvm::Value* Compiler::compilePrefixExpression(const AST::PrefixExpression* prefix)
{
    auto operand = compileExpression(prefix->right.get());
    if (!operand) return nullptr;

    if (prefix->op == "!")
    {
        // 真偽値を反転
        if (operand->getType()->isIntegerTy(1))
        {
            auto notResult = builder->CreateXor(operand, 
                llvm::ConstantInt::get(operand->getType(), 1));
            notResult->setName("not");
            // i1をi64に拡張
            return builder->CreateZExt(notResult, llvm::Type::getInt64Ty(*context));
        }
        else
        {
            // 64ビット整数を真偽値として扱う
            auto isZero = builder->CreateICmpEQ(operand, 
                llvm::ConstantInt::get(operand->getType(), 0));
            isZero->setName("not");
            // i1をi64に拡張
            return builder->CreateZExt(isZero, llvm::Type::getInt64Ty(*context));
        }
    }
    else if (prefix->op == "-")
    {
        return builder->CreateNeg(operand, "negtmp");
    }
    return nullptr;
}

llvm::Value* Compiler::compileBooleanLiteral(const AST::BooleanLiteral* boolean)
{
    auto value = llvm::ConstantInt::get(*context, llvm::APInt(1, boolean->value ? 1 : 0, false));
    return builder->CreateZExt(value, llvm::Type::getInt64Ty(*context));
}

llvm::Value* Compiler::compileIdentifier(const AST::Identifier* ident)
{
    auto it = namedValues.find(ident->value);
    if (it != namedValues.end())
    {
        return builder->CreateLoad(llvm::Type::getInt64Ty(*context), it->second, ident->value.c_str());
    }
    return nullptr;
}

llvm::Function* Compiler::createFunction(const std::string& name, 
                                       const std::vector<std::string>& argNames)
{
    std::vector<llvm::Type*> ints(argNames.size(), 
                                 llvm::Type::getInt64Ty(*context));
    
    llvm::FunctionType* ft = llvm::FunctionType::get(
        llvm::Type::getInt64Ty(*context), ints, false);
    
    llvm::Function* f = llvm::Function::Create(
        ft, llvm::Function::ExternalLinkage, name, module.get());
    
    unsigned idx = 0;
    for (auto& arg : f->args())
    {
        arg.setName(argNames[idx++]);
    }
    
    return f;
}

llvm::Value* Compiler::compileFunctionLiteral(const AST::FunctionLiteral* func)
{
    std::vector<std::string> argNames;
    for (const auto& param : func->parameters)
    {
        argNames.push_back(param->value);
    }
    
    std::string funcName = "anonymous_func_" + std::to_string(
        reinterpret_cast<std::uintptr_t>(func));
    llvm::Function* function = createFunction(funcName, argNames);
    
    // 現在のビル���ーの状態を保存
    auto prevBlock = builder->GetInsertBlock();
    auto prevFunc = builder->GetInsertBlock()->getParent();

    // 関数本体のコンパイル
    llvm::BasicBlock* bb = llvm::BasicBlock::Create(*context, "entry", function);
    builder->SetInsertPoint(bb);
    
    // 引数の割り当て
    namedValues.clear();
    for (auto& arg : function->args())
    {
        llvm::AllocaInst* alloca = createEntryBlockAlloca(
            function, std::string(arg.getName()));
        builder->CreateStore(&arg, alloca);
        namedValues[std::string(arg.getName())] = alloca;
    }
    
    // 関数本体のコンパイル
    if (func->body)
    {
        llvm::Value* lastValue = nullptr;
        for (const auto& stmt : func->body->statements)
        {
            lastValue = compileStatement(stmt.get());
        }
        
        // 戻り値の設定
        if (lastValue)
        {
            builder->CreateRet(lastValue);
        }
        else
        {
            builder->CreateRet(llvm::ConstantInt::get(*context, llvm::APInt(64, 0)));
        }
    }
    else
    {
        builder->CreateRet(llvm::ConstantInt::get(*context, llvm::APInt(64, 0)));
    }
    
    // 元のコンテキストに戻る
    builder->SetInsertPoint(prevBlock);

    return function;
}

llvm::AllocaInst* Compiler::createEntryBlockAlloca(llvm::Function* function,
                                                  const std::string& varName)
{
    llvm::IRBuilder<> tmpB(&function->getEntryBlock(),
                          function->getEntryBlock().begin());
    return tmpB.CreateAlloca(llvm::Type::getInt64Ty(*context),
                           nullptr, varName.c_str());
}

std::string Compiler::getIR() const
{
    std::string ir;
    llvm::raw_string_ostream os(ir);
    module->print(os, nullptr);
    return ir;
}

llvm::Value* Compiler::compileCallExpression(const AST::CallExpression* call)
{
    // 関数のコンパイル
    llvm::Value* callee = compileExpression(call->function.get());
    if (!callee) return nullptr;

    // 引数のコンパイル
    std::vector<llvm::Value*> args;
    for (const auto& arg : call->arguments)
    {
        if (auto compiled = compileExpression(arg.get()))
        {
            args.push_back(compiled);
        }
        else
        {
            return nullptr;
        }
    }

    // 関数呼び出し
    if (auto func = llvm::dyn_cast<llvm::Function>(callee))
    {
        if (func->arg_size() != args.size())
        {
            return nullptr; // 引数の数が一致しない
        }
        return builder->CreateCall(func, args, "calltmp");
    }

    return nullptr;
}

void Compiler::compileLetStatement(const AST::LetStatement* let)
{
    // 値の評価
    llvm::Value* value = nullptr;
    if (let->value)
    {
        value = compileExpression(let->value.get());
    }
    if (!value) return;

    // 変数のアロケーション
    llvm::Function* function = builder->GetInsertBlock()->getParent();
    llvm::AllocaInst* alloca = createEntryBlockAlloca(function, let->name->value);
    
    // 値の格納
    builder->CreateStore(value, alloca);
    namedValues[let->name->value] = alloca;
}

llvm::Value* Compiler::compileBlockStatement(const AST::BlockStatement* block)
{
    llvm::Value* lastValue = nullptr;
    for (const auto& stmt : block->statements)
    {
        lastValue = compileStatement(stmt.get());
        if (!lastValue) return nullptr;
    }
    return lastValue;
}

llvm::Value* Compiler::compileIfExpression(const AST::IfExpression* ifExpr)
{
    // 条件式のコンパイル
    auto condition = compileExpression(ifExpr->getCondition());
    if (!condition) return nullptr;

    // 現在の関数を取得
    llvm::Function* function = builder->GetInsertBlock()->getParent();

    // 各ブロックの作成（すべてのブロックを関数に直接追加）
    llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(*context, "then", function);
    llvm::BasicBlock* elseBB = ifExpr->getAlternative() 
        ? llvm::BasicBlock::Create(*context, "else", function) 
        : nullptr;
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context, "ifcont", function);

    // 条件分岐の生成
    builder->CreateCondBr(condition, thenBB, elseBB ? elseBB : mergeBB);

    // then部分のコンパイル
    builder->SetInsertPoint(thenBB);
    auto thenVal = compileBlockStatement(ifExpr->getConsequence());
    if (!thenVal) return nullptr;
    builder->CreateBr(mergeBB);

    // else部分のコンパイル（存在する場合）
    llvm::Value* elseVal = nullptr;
    if (elseBB) {
        builder->SetInsertPoint(elseBB);
        elseVal = compileBlockStatement(ifExpr->getAlternative());
        if (!elseVal) return nullptr;
        builder->CreateBr(mergeBB);
    }

    // マージブロックの処理
    builder->SetInsertPoint(mergeBB);
    
    if (elseVal) {
        llvm::PHINode* phi = builder->CreatePHI(thenVal->getType(), 2, "iftmp");
        phi->addIncoming(thenVal, thenBB);
        phi->addIncoming(elseVal, elseBB);
        return phi;
    }
    
    return thenVal;
}

llvm::Value* Compiler::compileWhileExpression(const AST::WhileExpression* whileExpr)
{
    // 現在の関数を取得
    llvm::Function* function = builder->GetInsertBlock()->getParent();

    // ブロックの作成（すべてのブロックを関数に直接追加）
    llvm::BasicBlock* condBB = llvm::BasicBlock::Create(*context, "cond", function);
    llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(*context, "loop", function);
    llvm::BasicBlock* afterBB = llvm::BasicBlock::Create(*context, "afterloop", function);

    // 条件ブロックにジャンプ
    builder->CreateBr(condBB);

    // 条件式のコンパイル
    builder->SetInsertPoint(condBB);
    auto condition = compileExpression(whileExpr->getCondition());
    if (!condition) return nullptr;
    builder->CreateCondBr(condition, loopBB, afterBB);

    // ループ本体のコンパイル
    builder->SetInsertPoint(loopBB);
    auto bodyVal = compileBlockStatement(whileExpr->getBody());
    if (!bodyVal) return nullptr;
    builder->CreateBr(condBB);

    // ループ後のブロック
    builder->SetInsertPoint(afterBB);

    return llvm::ConstantInt::get(*context, llvm::APInt(64, 0));
}

} // namespace JIT