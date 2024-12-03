#include "token.hpp"

namespace Token {

namespace {
    const std::unordered_map<std::string, TokenType> keywords = {
        {"fn", TokenType::FUNCTION},
        {"let", TokenType::LET},
        {"true", TokenType::TRUE},
        {"false", TokenType::FALSE},
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"return", TokenType::RETURN},
        {"while", TokenType::WHILE},
        {"for", TokenType::FOR}
    };
}

std::string toString(TokenType type) {
    switch (type) {
        case TokenType::ILLEGAL: return "ILLEGAL";
        case TokenType::EOF_: return "EOF";
        case TokenType::IDENT: return "IDENT";
        case TokenType::INT: return "INT";
        case TokenType::STRING: return "STRING";
        case TokenType::ASSIGN: return "=";
        case TokenType::PLUS: return "+";
        case TokenType::MINUS: return "-";
        case TokenType::BANG: return "!";
        case TokenType::ASTERISK: return "*";
        case TokenType::SLASH: return "/";
        case TokenType::LT: return "<";
        case TokenType::GT: return ">";
        case TokenType::EQ: return "==";
        case TokenType::NOT_EQ: return "!=";
        case TokenType::COMMA: return ",";
        case TokenType::SEMICOLON: return ";";
        case TokenType::LPAREN: return "(";
        case TokenType::RPAREN: return ")";
        case TokenType::LBRACE: return "{";
        case TokenType::RBRACE: return "}";
        case TokenType::LBRACKET: return "[";
        case TokenType::RBRACKET: return "]";
        case TokenType::FUNCTION: return "fn";
        case TokenType::LET: return "let";
        case TokenType::TRUE: return "true";
        case TokenType::FALSE: return "false";
        case TokenType::IF: return "if";
        case TokenType::ELSE: return "else";
        case TokenType::RETURN: return "return";
        case TokenType::WHILE: return "while";
        case TokenType::FOR: return "for";
        default: return "UNKNOWN";
    }
}

Token::Token(TokenType type, std::string literal) 
    : type_(type), literal_(std::move(literal)) {}

std::string Token::getTypeString() const {
    return toString(type_);
}

bool Token::operator==(const Token& other) const {
    return type_ == other.type_ && literal_ == other.literal_;
}

bool Token::operator==(const TokenType& other) const {
    return type_ == other;
}

bool Token::operator!=(const Token& other) const {
    return !(*this == other);
}

bool Token::operator!=(const TokenType& other) const {
    return type_ != other;
}

bool Token::isKeyword(const std::string& word) {
    return keywords.find(word) != keywords.end();
}

TokenType Token::lookupKeyword(const std::string& word) {
    auto it = keywords.find(word);
    return it != keywords.end() ? it->second : TokenType::IDENT;
}

} // namespace Token