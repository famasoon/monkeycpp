#include "jit.hpp"
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/Scalar/GVN.h>

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
    llvm::PassBuilder passBuilder;

    // 最適化パイプラインの設定
    llvm::LoopAnalysisManager loopAnalysisManager;
    llvm::FunctionAnalysisManager functionAnalysisManager;
    llvm::CGSCCAnalysisManager cGSCCAnalysisManager;

    // 分析マネージャーの登録
    passBuilder.registerModuleAnalyses(moduleAnalysisManager);
    passBuilder.registerCGSCCAnalyses(cGSCCAnalysisManager);
    passBuilder.registerFunctionAnalyses(functionAnalysisManager);
    passBuilder.registerLoopAnalyses(loopAnalysisManager);
    passBuilder.crossRegisterProxies(loopAnalysisManager, functionAnalysisManager,
                                   cGSCCAnalysisManager, moduleAnalysisManager);

    // 最適化レベルに応じたパスの設定
    switch (level)
    {
        case 0:
            passManager = passBuilder.buildO0DefaultPipeline(llvm::OptimizationLevel::O0);
            break;
        default:
            passManager = passBuilder.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O2);
    }

    functionPassManager = passBuilder.buildFunctionSimplificationPipeline(
        llvm::OptimizationLevel::O2, llvm::ThinOrFullLTOPhase::None);
}

void Compiler::compile(const AST::Program& program)
{
    for (const auto& stmt : program.statements)
    {
        if (stmt)
        {
            compileStatement(stmt.get());
        }
    }
    
    // コンパイル後に最適化を実行
    runOptimizations();
}

void Compiler::runOptimizations()
{
    // モジュールレベルの最適化を実行
    passManager.run(*module, moduleAnalysisManager);
}

void Compiler::compileStatement(const AST::Statement *stmt)
{
    if (auto exprStmt = dynamic_cast<const AST::ExpressionStatement *>(stmt))
    {
        if (exprStmt->expression)
        {
            compileExpression(exprStmt->expression.get());
        }
    }
    // 他の文型のハンドリングを追加
}

llvm::Value *Compiler::compileExpression(const AST::Expression *expr)
{
    if (auto intLiteral = dynamic_cast<const AST::IntegerLiteral *>(expr))
    {
        return compileIntegerLiteral(intLiteral);
    }
    if (auto infixExpr = dynamic_cast<const AST::InfixExpression *>(expr))
    {
        return compileInfixExpression(infixExpr);
    }
    if (auto prefixExpr = dynamic_cast<const AST::PrefixExpression *>(expr))
    {
        return compilePrefixExpression(prefixExpr);
    }
    if (auto booleanLiteral = dynamic_cast<const AST::BooleanLiteral *>(expr))
    {
        return compileBooleanLiteral(booleanLiteral);
    }
    if (auto identifier = dynamic_cast<const AST::Identifier *>(expr))
    {
        return compileIdentifier(identifier);
    }
    if (auto functionLiteral = dynamic_cast<const AST::FunctionLiteral *>(expr))
    {
        return compileFunctionLiteral(functionLiteral);
    }
    if (auto callExpr = dynamic_cast<const AST::CallExpression *>(expr))
    {
        return compileCallExpression(callExpr);
    }
    // 他の式型のハンドリングを追加
    return nullptr;
}

llvm::Value *Compiler::compileIntegerLiteral(const AST::IntegerLiteral *literal)
{
    return llvm::ConstantInt::get(*context, llvm::APInt(64, literal->value, true));
}

llvm::Value *Compiler::compileInfixExpression(const AST::InfixExpression *infix)
{
    auto left = compileExpression(infix->left.get());
    auto right = compileExpression(infix->right.get());

    if (!left || !right)
        return nullptr;

    if (infix->op == "+")
    {
        return builder->CreateAdd(left, right, "addtmp");
    }
    else if (infix->op == "-")
    {
        return builder->CreateSub(left, right, "subtmp");
    }
    else if (infix->op == "*")
    {
        return builder->CreateMul(left, right, "multmp");
    }
    else if (infix->op == "/")
    {
        return builder->CreateSDiv(left, right, "divtmp");
    }

    return nullptr;
}

llvm::Value* Compiler::compilePrefixExpression(const AST::PrefixExpression* prefix)
{
    auto operand = compileExpression(prefix->right.get());
    if (!operand) return nullptr;

    if (prefix->op == "!")
    {
        return builder->CreateNot(operand, "nottmp");
    }
    else if (prefix->op == "-")
    {
        return builder->CreateNeg(operand, "negtmp");
    }
    return nullptr;
}

llvm::Value* Compiler::compileBooleanLiteral(const AST::BooleanLiteral* boolean)
{
    return llvm::ConstantInt::get(*context, llvm::APInt(1, boolean->value ? 1 : 0, false));
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
        for (const auto& stmt : func->body->statements)
        {
            compileStatement(stmt.get());
        }
    }
    
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

} // namespace JIT