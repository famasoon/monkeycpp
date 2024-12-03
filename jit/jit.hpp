#pragma once
#include "../ast/ast.hpp"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <memory>

namespace JIT
{

class Compiler
{
  private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;

  public:
    Compiler();
    ~Compiler() = default;

    // ASTからLLVM IRを生成
    void compile(const AST::Program &program);

  private:
    // 各種ASTノードのコンパイル
    llvm::Value *compileExpression(const AST::Expression *expr);
    void compileStatement(const AST::Statement *stmt);
    llvm::Value *compileIntegerLiteral(const AST::IntegerLiteral *literal);
    llvm::Value *compileInfixExpression(const AST::InfixExpression *infix);
};

} // namespace JIT