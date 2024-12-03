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
    llvm::ModuleAnalysisManager moduleAnalysisManager;

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
        case 0: // -O0
            passManager = passBuilder.buildO0DefaultPipeline(llvm::OptimizationLevel::O0);
            break;
        case 1: // -O1
            passManager = passBuilder.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O1);
            break;
        case 2: // -O2
            passManager = passBuilder.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O2);
            break;
        case 3: // -O3
            passManager = passBuilder.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O3);
            break;
        default:
            passManager = passBuilder.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O2);
    }

    // 関数レベルの最適化パスの設定
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

} // namespace JIT