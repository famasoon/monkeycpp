#pragma once
#include <string>

namespace Token
{

// トークンの種類を表す列挙型
enum class TokenType
{
    ILLEGAL,
    EOF_,

    // 識別子 + リテラル
    IDENT,
    INT,
    STRING,

    // 演算子
    ASSIGN,
    PLUS,
    MINUS,
    BANG,
    ASTERISK,
    SLASH,

    LT,
    GT,
    EQ,
    NOT_EQ,

    // デリミタ
    COMMA,
    SEMICOLON,

    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,

    // キーワード
    FUNCTION,
    LET,
    TRUE,
    FALSE,
    IF,
    ELSE,
    RETURN,

    LBRACKET, // [
    RBRACKET, // ]
};

// TokenTypeを文字列に変換する関数
std::string toString(TokenType type);

class Token
{
  public:
    TokenType type;
    std::string literal;

    Token(TokenType type, std::string literal);
    bool operator==(const Token &other) const;
    bool operator==(const TokenType &other) const;
    bool operator!=(const Token &other) const;
    bool operator!=(const TokenType &other) const;
    std::string getType() const;

    // トークン文字列の定数
    static const std::string ILLEGAL;
    static const std::string EOF_;
    static const std::string IDENT;
    static const std::string INT;
    static const std::string ASSIGN;
    static const std::string PLUS;
    static const std::string MINUS;
    static const std::string BANG;
    static const std::string ASTERISK;
    static const std::string SLASH;
    static const std::string LT;
    static const std::string GT;
    static const std::string EQ;
    static const std::string NOT_EQ;
    static const std::string COMMA;
    static const std::string SEMICOLON;
    static const std::string LPAREN;
    static const std::string RPAREN;
    static const std::string LBRACE;
    static const std::string RBRACE;
    static const std::string FUNCTION;
    static const std::string LET;
    static const std::string TRUE;
    static const std::string FALSE;
    static const std::string IF;
    static const std::string ELSE;
    static const std::string RETURN;
    static const std::string LBRACKET;
    static const std::string RBRACKET;
};

} // namespace Token
