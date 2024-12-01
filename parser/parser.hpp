#pragma once
#include "../lexer/lexer.hpp"
#include "../ast/ast.hpp"
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>

namespace Parser {

class Parser {
public:
    explicit Parser(Lexer::Lexer lexer);
    std::unique_ptr<AST::Program> ParseProgram();
    const std::vector<std::string>& Errors() const { return errors; }

private:
    Lexer::Lexer lexer;
    Token::Token curToken;
    Token::Token peekToken;
    std::vector<std::string> errors;

    // 優先順位の定義
    enum class Precedence {
        LOWEST,
        EQUALS,      // ==
        LESSGREATER, // > または <
        SUM,         // +
        PRODUCT,     // *
        PREFIX,      // -X または !X
        CALL         // myFunction(X)
    };

    void nextToken();
    bool curTokenIs(Token::TokenType t) const;
    bool peekTokenIs(Token::TokenType t) const;
    bool expectPeek(Token::TokenType t);
    void peekError(Token::TokenType t);
    void registerError(const std::string& msg);

    // 文のパース
    std::unique_ptr<AST::Statement> parseStatement();
    std::unique_ptr<AST::LetStatement> parseLetStatement();
    std::unique_ptr<AST::ReturnStatement> parseReturnStatement();
    std::unique_ptr<AST::ExpressionStatement> parseExpressionStatement();

    // 式のパース
    std::unique_ptr<AST::Expression> parseExpression(Precedence precedence);
    std::unique_ptr<AST::Expression> parseIdentifier();
};

} // namespace Parser
