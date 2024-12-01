#include <gtest/gtest.h>
#include "../parser/parser.hpp"

TEST(ParserTest, TestLetStatements) {
    struct TestCase {
        std::string input;
        std::string expectedIdentifier;
    };

    std::vector<TestCase> tests = {
        {"let x = 5;", "x"},
        {"let y = 10;", "y"},
        {"let foobar = 838383;", "foobar"},
    };

    for (const auto& tt : tests) {
        Lexer::Lexer lexer(tt.input);
        Parser::Parser parser(lexer);
        auto program = parser.ParseProgram();

        ASSERT_NE(program, nullptr) << "ParseProgram() returned nullptr";
        
        const auto& errors = parser.Errors();
        ASSERT_TRUE(errors.empty()) << "parser has " << errors.size() << " errors";
        
        ASSERT_EQ(program->statements.size(), 1) 
            << "program.statements does not contain 1 statement. got="
            << program->statements.size();

        auto stmt = dynamic_cast<AST::LetStatement*>(program->statements[0].get());
        ASSERT_NE(stmt, nullptr) << "program.statements[0] is not LetStatement";
        
        EXPECT_EQ(stmt->TokenLiteral(), "let") 
            << "stmt.TokenLiteral not 'let'. got=" << stmt->TokenLiteral();
        
        EXPECT_EQ(stmt->name->value, tt.expectedIdentifier)
            << "letStmt.name.value not '" << tt.expectedIdentifier 
            << "'. got=" << stmt->name->value;
        
        EXPECT_EQ(stmt->name->TokenLiteral(), tt.expectedIdentifier)
            << "letStmt.name.TokenLiteral() not '" << tt.expectedIdentifier 
            << "'. got=" << stmt->name->TokenLiteral();
    }
}

TEST(ParserTest, TestReturnStatements) {
    std::string input = R"(
        return 5;
        return 10;
        return 993322;
    )";

    Lexer::Lexer lexer(input);
    Parser::Parser parser(lexer);
    auto program = parser.ParseProgram();

    ASSERT_NE(program, nullptr) << "ParseProgram() returned nullptr";
    
    const auto& errors = parser.Errors();
    ASSERT_TRUE(errors.empty()) << "parser has " << errors.size() << " errors";
    
    ASSERT_EQ(program->statements.size(), 3) 
        << "program.statements does not contain 3 statements. got="
        << program->statements.size();

    for (const auto& stmt : program->statements) {
        auto returnStmt = dynamic_cast<AST::ReturnStatement*>(stmt.get());
        ASSERT_NE(returnStmt, nullptr) << "stmt not ReturnStatement";
        EXPECT_EQ(returnStmt->TokenLiteral(), "return")
            << "returnStmt.TokenLiteral not 'return'. got="
            << returnStmt->TokenLiteral();
    }
} 