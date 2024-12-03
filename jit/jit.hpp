#pragma once
#include "../ast/ast.hpp"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/PassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <memory>
#include <unordered_map>
#include <llvm/IR/Verifier.h>

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
    
    // シンボルテーブル
    std::unordered_map<std::string, llvm::Value*> namedValues;
    // 現在のコンパイル中の関数
    llvm::Function* currentFunction;

    // 新しいメソッドの宣言を追加
    llvm::Value* compileIfExpression(const AST::IfExpression* ifExpr);
    llvm::Value* compileWhileExpression(const AST::WhileExpression* whileExpr);
    llvm::Value* compileBlockStatement(const AST::BlockStatement* block);

  public:
    Compiler();
    ~Compiler() = default;

    void compile(const AST::Program& program);
    void setOptimizationLevel(unsigned level);
    
    // IRの文字列表現を取得
    std::string getIR() const;

  private:
    void initializeOptimizations(unsigned level);
    void runOptimizations();

    llvm::Value* compileExpression(const AST::Expression* expr);
    llvm::Value* compileStatement(const AST::Statement* stmt);
    
    // 新しいコンパイルメソッド
    llvm::Value* compileIntegerLiteral(const AST::IntegerLiteral* literal);
    llvm::Value* compileInfixExpression(const AST::InfixExpression* infix);
    llvm::Value* compilePrefixExpression(const AST::PrefixExpression* prefix);
    llvm::Value* compileIdentifier(const AST::Identifier* ident);
    llvm::Value* compileFunctionLiteral(const AST::FunctionLiteral* func);
    llvm::Value* compileCallExpression(const AST::CallExpression* call);
    llvm::Value* compileBooleanLiteral(const AST::BooleanLiteral* boolean);
    
    // 文のコンパイル
    void compileLetStatement(const AST::LetStatement* let);
    void compileReturnStatement(const AST::ReturnStatement* ret);
    void compileExpressionStatement(const AST::ExpressionStatement* expr);
    
    // ユーティリティ関数
    llvm::Function* createFunction(const std::string& name, 
                                 const std::vector<std::string>& argNames);
    llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* function,
                                           const std::string& varName);
};

} // namespace JIT