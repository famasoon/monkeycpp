#pragma once
#include <string>
#include <unordered_map>

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
    WHILE,
    FOR,
};

// TokenTypeを文字列に変換する関数
std::string toString(TokenType type);

class Token
{
  private:
    TokenType type_;
    std::string literal_;

  public:
    Token(TokenType type, std::string literal);

    // アクセサ
    TokenType getType() const { return type_; }
    const std::string& getLiteral() const { return literal_; }
    std::string getTypeString() const;

    // 比較演算子
    bool operator==(const Token& other) const;
    bool operator==(const TokenType& other) const;
    bool operator!=(const Token& other) const;
    bool operator!=(const TokenType& other) const;

    // キーワードの判定
    static bool isKeyword(const std::string& word);
    static TokenType lookupKeyword(const std::string& word);
};

} // namespace Token
