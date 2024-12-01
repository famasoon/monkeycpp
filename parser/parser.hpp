#pragma once
#include "../lexer/lexer.hpp"
#include "../ast/ast.hpp"
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <unordered_map>
#include <iostream>

namespace Parser
{

  class Parser
  {
    using PrefixParseFn = std::function<std::unique_ptr<AST::Expression>()>;
    using InfixParseFn = std::function<std::unique_ptr<AST::Expression>(std::unique_ptr<AST::Expression>)>;

  public:
    explicit Parser(Lexer::Lexer lexer);
    std::unique_ptr<AST::Program> ParseProgram();
    const std::vector<std::string> &Errors() const { return errors; }

    // デバッグ設定
    void setDebugMode(bool enable) { debugMode = enable; }
    void setDebugOutput(std::ostream &out) { debugOut = &out; }

  private:
    Lexer::Lexer lexer;
    Token::Token curToken;
    Token::Token peekToken;
    std::vector<std::string> errors;
    std::unordered_map<Token::TokenType, PrefixParseFn> prefixParseFns;
    std::unordered_map<Token::TokenType, InfixParseFn> infixParseFns;

    // デバッグ関連
    bool debugMode = false;
    std::ostream *debugOut = &std::cout;
    int indentLevel = 0;
    void trace(const std::string &msg);
    void increaseIndent() { indentLevel++; }
    void decreaseIndent() { indentLevel--; }
    std::string getIndent() const { return std::string(indentLevel * 2, ' '); }

    // 優先順位の定義
    enum class Precedence
    {
      LOWEST = 1,
      EQUALS,      // ==
      LESSGREATER, // > または <
      SUM,         // +
      PRODUCT,     // *
      PREFIX,      // -X または !X
      CALL         // myFunction(X)
    };

    static std::unordered_map<Token::TokenType, Precedence> precedences;

    // パーサー初期化
    void initParseFns();

    // パーサー登録
    void registerPrefix(Token::TokenType type, PrefixParseFn fn);
    void registerInfix(Token::TokenType type, InfixParseFn fn);

    // 優先順位関連
    Precedence peekPrecedence() const;
    Precedence curPrecedence() const;

    // トークン処理
    void nextToken();
    bool curTokenIs(Token::TokenType t) const;
    bool peekTokenIs(Token::TokenType t) const;
    bool expectPeek(Token::TokenType t);

    // エラー処理
    void peekError(Token::TokenType t);
    void registerError(const std::string &msg);
    void noPrefixParseFnError(Token::TokenType t);

    // 文のパース
    std::unique_ptr<AST::Statement> parseStatement();
    std::unique_ptr<AST::LetStatement> parseLetStatement();
    std::unique_ptr<AST::ReturnStatement> parseReturnStatement();
    std::unique_ptr<AST::ExpressionStatement> parseExpressionStatement();

    // 式のパース
    std::unique_ptr<AST::Expression> parseExpression(Precedence precedence);
    std::unique_ptr<AST::Expression> parseIdentifier();
    std::unique_ptr<AST::Expression> parseIntegerLiteral();
    std::unique_ptr<AST::Expression> parsePrefixExpression();
    std::unique_ptr<AST::Expression> parseInfixExpression(std::unique_ptr<AST::Expression> left);
    std::unique_ptr<AST::Expression> parseFunctionLiteral();
    std::unique_ptr<AST::BlockStatement> parseBlockStatement();
    std::vector<std::unique_ptr<AST::Identifier>> parseFunctionParameters();
  };

} // namespace Parser
