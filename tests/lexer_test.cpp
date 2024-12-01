#include <gtest/gtest.h>
#include "../lexer/lexer.hpp"

TEST(LexerTest, TestNextToken) {
    std::string input = R"(
        let five = 5;
        let ten = 10;
        let add = fn(x, y) {
            x + y;
        };
        let result = add(five, ten);
        !-/*5;
        5 < 10 > 5;
        if (5 < 10) {
            return true;
        } else {
            return false;
        }
        10 == 10;
        10 != 9;
    )";

    struct TestCase {
        Token::TokenType expectedType;
        std::string expectedLiteral;
    };

    std::vector<TestCase> tests = {
        {Token::TokenType::LET, "let"},
        {Token::TokenType::IDENT, "five"},
        {Token::TokenType::ASSIGN, "="},
        {Token::TokenType::INT, "5"},
        {Token::TokenType::SEMICOLON, ";"},
        {Token::TokenType::LET, "let"},
        {Token::TokenType::IDENT, "ten"},
        {Token::TokenType::ASSIGN, "="},
        {Token::TokenType::INT, "10"},
        {Token::TokenType::SEMICOLON, ";"},
        {Token::TokenType::LET, "let"},
        {Token::TokenType::IDENT, "add"},
        {Token::TokenType::ASSIGN, "="},
        {Token::TokenType::FUNCTION, "fn"},
        {Token::TokenType::LPAREN, "("},
        {Token::TokenType::IDENT, "x"},
        {Token::TokenType::COMMA, ","},
        {Token::TokenType::IDENT, "y"},
        {Token::TokenType::RPAREN, ")"},
        {Token::TokenType::LBRACE, "{"},
        {Token::TokenType::IDENT, "x"},
        {Token::TokenType::PLUS, "+"},
        {Token::TokenType::IDENT, "y"},
        {Token::TokenType::SEMICOLON, ";"},
        {Token::TokenType::RBRACE, "}"},
        {Token::TokenType::SEMICOLON, ";"},
        {Token::TokenType::LET, "let"},
        {Token::TokenType::IDENT, "result"},
        {Token::TokenType::ASSIGN, "="},
        {Token::TokenType::IDENT, "add"},
        {Token::TokenType::LPAREN, "("},
        {Token::TokenType::IDENT, "five"},
        {Token::TokenType::COMMA, ","},
        {Token::TokenType::IDENT, "ten"},
        {Token::TokenType::RPAREN, ")"},
        {Token::TokenType::SEMICOLON, ";"},
        {Token::TokenType::BANG, "!"},
        {Token::TokenType::MINUS, "-"},
        {Token::TokenType::SLASH, "/"},
        {Token::TokenType::ASTERISK, "*"},
        {Token::TokenType::INT, "5"},
        {Token::TokenType::SEMICOLON, ";"},
        {Token::TokenType::INT, "5"},
        {Token::TokenType::LT, "<"},
        {Token::TokenType::INT, "10"},
        {Token::TokenType::GT, ">"},
        {Token::TokenType::INT, "5"},
        {Token::TokenType::SEMICOLON, ";"},
        {Token::TokenType::IF, "if"},
        {Token::TokenType::LPAREN, "("},
        {Token::TokenType::INT, "5"},
        {Token::TokenType::LT, "<"},
        {Token::TokenType::INT, "10"},
        {Token::TokenType::RPAREN, ")"},
        {Token::TokenType::LBRACE, "{"},
        {Token::TokenType::RETURN, "return"},
        {Token::TokenType::TRUE, "true"},
        {Token::TokenType::SEMICOLON, ";"},
        {Token::TokenType::RBRACE, "}"},
        {Token::TokenType::ELSE, "else"},
        {Token::TokenType::LBRACE, "{"},
        {Token::TokenType::RETURN, "return"},
        {Token::TokenType::FALSE, "false"},
        {Token::TokenType::SEMICOLON, ";"},
        {Token::TokenType::RBRACE, "}"},
        {Token::TokenType::INT, "10"},
        {Token::TokenType::EQ, "=="},
        {Token::TokenType::INT, "10"},
        {Token::TokenType::SEMICOLON, ";"},
        {Token::TokenType::INT, "10"},
        {Token::TokenType::NOT_EQ, "!="},
        {Token::TokenType::INT, "9"},
        {Token::TokenType::SEMICOLON, ";"},
        {Token::TokenType::EOF_, ""},
    };

    Lexer::Lexer lexer(input);

    for (size_t i = 0; i < tests.size(); i++) {
        auto tok = lexer.NextToken();
        
        EXPECT_EQ(tok.type, tests[i].expectedType) 
            << "tests[" << i << "] - tokentype wrong. "
            << "expected=" << Token::toString(tests[i].expectedType)
            << ", got=" << Token::toString(tok.type);
        
        EXPECT_EQ(tok.literal, tests[i].expectedLiteral)
            << "tests[" << i << "] - literal wrong. "
            << "expected=" << tests[i].expectedLiteral
            << ", got=" << tok.literal;
    }
} 