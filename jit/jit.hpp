#pragma once
#include "../ast/ast.hpp"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <memory>

namespace JIT
{

class Compiler
{
  private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;
    llvm::ModulePassManager passManager;
    llvm::FunctionPassManager functionPassManager;
    llvm::ModuleAnalysisManager moduleAnalysisManager;

  public:
    Compiler();
    ~Compiler() = default;

    // ASTからLLVM IRを生成
    void compile(const AST::Program &program);

    // 最適化レベルを設定
    void setOptimizationLevel(unsigned level);

  private:
    // 最適化の初期化
    void initializeOptimizations(unsigned level);
    // モジュールレベルの最適化を実行
    void runOptimizations();

    // 各種ASTノードのコンパイル
    llvm::Value *compileExpression(const AST::Expression *expr);
    void compileStatement(const AST::Statement *stmt);
    llvm::Value *compileIntegerLiteral(const AST::IntegerLiteral *literal);
    llvm::Value *compileInfixExpression(const AST::InfixExpression *infix);
};

} // namespace JIT