#include "jit.hpp"
#include <llvm/Support/raw_ostream.h>

namespace JIT
{

Compiler::Compiler()
{
    context = std::make_unique<llvm::LLVMContext>();
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    module = std::make_unique<llvm::Module>("monkey_jit", *context);
}

void Compiler::compile(const AST::Program &program)
{
    for (const auto &stmt : program.statements)
    {
        if (stmt)
        {
            compileStatement(stmt.get());
        }
    }
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