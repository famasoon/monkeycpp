#include "../jit/jit.hpp"
#include "../parser/parser.hpp"
#include <gtest/gtest.h>

class JITTest : public ::testing::Test
{
protected:
    JIT::Compiler compiler;
    
    AST::Program* parseProgram(const std::string& input)
    {
        auto lexer = std::make_unique<Lexer::Lexer>(input);
        Parser::Parser parser(std::move(lexer));
        return parser.ParseProgram().release();
    }
};

TEST_F(JITTest, TestIntegerArithmetic)
{
    try {
        std::unique_ptr<AST::Program> program(parseProgram("5 + 3 * 2"));
        ASSERT_TRUE(program != nullptr) << "Failed to parse program";
        
        compiler.compile(*program);
        
        std::string ir = compiler.getIR();
        std::cout << "Generated IR:\n" << ir << std::endl;
        
        EXPECT_TRUE(ir.find("ret i64 11") != std::string::npos) 
            << "Expected result value not found";
    } catch (const std::exception& e) {
        FAIL() << "Caught exception: " << e.what();
    }
}

TEST_F(JITTest, TestSimpleFunction)
{
    std::unique_ptr<AST::Program> program(parseProgram(
        "let add = fn(x, y) { x + y; };"
    ));
    compiler.compile(*program);
    
    std::string ir = compiler.getIR();
    EXPECT_TRUE(ir.find("define") != std::string::npos);
    EXPECT_TRUE(ir.find("add") != std::string::npos);
}

TEST_F(JITTest, TestBooleanOperations)
{
    try {
        std::unique_ptr<AST::Program> program(parseProgram("!true"));
        ASSERT_TRUE(program != nullptr) << "Failed to parse program";
        
        compiler.compile(*program);
        
        std::string ir = compiler.getIR();
        std::cout << "Generated IR:\n" << ir << std::endl;
        
        EXPECT_TRUE(ir.find("ret i64 0") != std::string::npos) 
            << "Expected result value not found";
    } catch (const std::exception& e) {
        FAIL() << "Caught exception: " << e.what();
    }
}

TEST_F(JITTest, TestVariableBinding)
{
    std::unique_ptr<AST::Program> program(parseProgram(
        "let x = 42; x;"
    ));
    compiler.compile(*program);
    
    std::string ir = compiler.getIR();
    EXPECT_TRUE(ir.find("alloca") != std::string::npos);
    EXPECT_TRUE(ir.find("store") != std::string::npos);
    EXPECT_TRUE(ir.find("load") != std::string::npos);
} 