#include <gtest/gtest.h>
#include "../parser/parser.hpp"
#include "../lexer/lexer.hpp"

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

TEST(ParserTest, TestLetStatementErrors) {
    const std::string input = R"(
        let = 5;
        let x 5;
        let 838383;
    )";

    Lexer::Lexer l(input);
    Parser::Parser p(l);

    auto program = p.ParseProgram();
    auto errors = p.Errors();

    // エラーが3つあることを確認
    ASSERT_EQ(errors.size(), 3);

    // 期待されるエラーメッセージをチェック
    std::vector<std::string> expectedErrors = {
        "expected next token to be IDENT, got ASSIGN instead",
        "expected next token to be ASSIGN, got INT instead",
        "expected next token to be IDENT, got INT instead"
    };

    for (size_t i = 0; i < errors.size(); i++) {
        EXPECT_EQ(errors[i], expectedErrors[i]);
    }
}

TEST(ParserTest, TestInvalidExpressionError) {
    const std::string input = "!;";  // 無効な前置演算子の使用

    Lexer::Lexer l(input);
    Parser::Parser p(l);

    auto program = p.ParseProgram();
    auto errors = p.Errors();

    ASSERT_EQ(errors.size(), 1);
    EXPECT_EQ(errors[0], "no prefix parse function for BANG found");
}

TEST(ParserTest, TestMissingSemicolonError) {
    const std::string input = R"(
        let x = 5
        let y = 10;
    )";

    Lexer::Lexer l(input);
    Parser::Parser p(l);

    auto program = p.ParseProgram();
    auto errors = p.Errors();

    ASSERT_GE(errors.size(), 1);
    // セミコロンが必要なことを示すエラーメッセージが含まれているか確認
    bool hasSemicolonError = false;
    for (const auto& error : errors) {
        if (error.find("semicolon") != std::string::npos) {
            hasSemicolonError = true;
            break;
        }
    }
    EXPECT_TRUE(hasSemicolonError);
}

TEST(ParserTest, TestReturnStatementError) {
    const std::string input = "return";  // 値が指定されていない

    Lexer::Lexer l(input);
    Parser::Parser p(l);

    auto program = p.ParseProgram();
    auto errors = p.Errors();

    ASSERT_EQ(errors.size(), 1);
    EXPECT_TRUE(errors[0].find("return") != std::string::npos);
}

TEST(ParserTest, TestMultipleErrors) {
    const std::string input = R"(
        let x 5;
        return;
        let = 10;
        let 838383;
    )";

    Lexer::Lexer l(input);
    Parser::Parser p(l);

    auto program = p.ParseProgram();
    auto errors = p.Errors();

    // 複数のエラーが検出されることを確認
    ASSERT_GT(errors.size(), 2);

    // エラーメッセージの内容をチェック
    bool hasAssignError = false;
    bool hasIdentError = false;

    for (const auto& error : errors) {
        if (error.find("ASSIGN") != std::string::npos) {
            hasAssignError = true;
        }
        if (error.find("IDENT") != std::string::npos) {
            hasIdentError = true;
        }
    }

    EXPECT_TRUE(hasAssignError);
    EXPECT_TRUE(hasIdentError);
} 