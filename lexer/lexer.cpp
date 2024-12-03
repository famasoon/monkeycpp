#include "lexer.hpp"
#include <cctype>
#include <stdexcept>
#include <unordered_map>

namespace Lexer
{

namespace
{
const std::unordered_map<std::string, Token::TokenType> keywords = {
    {"fn", Token::TokenType::FUNCTION},  {"let", Token::TokenType::LET},
    {"true", Token::TokenType::TRUE},    {"false", Token::TokenType::FALSE},
    {"if", Token::TokenType::IF},        {"else", Token::TokenType::ELSE},
    {"return", Token::TokenType::RETURN}, {"while", Token::TokenType::WHILE},
    {"for", Token::TokenType::FOR}};
}

Lexer::Lexer(std::string input) : input(std::move(input)), position(0), readPosition(0), ch(0)
{
    readChar();
}

void Lexer::readChar()
{
    ch = (readPosition >= input.length()) ? 0 : input[readPosition];
    position = readPosition++;
}

char Lexer::peekChar() const
{
    return (readPosition >= input.length()) ? 0 : input[readPosition];
}

std::string Lexer::readIdentifier()
{
    auto pos = position;
    while (std::isalpha(ch) || ch == '_')
    {
        readChar();
    }
    return input.substr(pos, position - pos);
}

std::string Lexer::readNumber()
{
    auto pos = position;
    while (std::isdigit(ch))
    {
        readChar();
    }
    return input.substr(pos, position - pos);
}

void Lexer::skipWhitespace()
{
    while (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r')
    {
        readChar();
    }
}

Token::TokenType Lexer::lookupIdent(const std::string &ident) const
{
    auto it = keywords.find(ident);
    return it != keywords.end() ? it->second : Token::TokenType::IDENT;
}

std::string Lexer::readString()
{
    readChar(); // 開始の'"'をスキップ

    auto startPosition = position;
    while (ch != 0 && ch != '"')
    {
        readChar();
    }

    if (ch == 0)
    {
        throw std::runtime_error("unterminated string literal");
    }

    std::string str = input.substr(startPosition, position - startPosition);
    readChar(); // 終わりの'"'をスキップ
    return str;
}

Token::Token Lexer::NextToken()
{
    skipWhitespace();

    Token::Token tok(Token::TokenType::ILLEGAL, std::string(1, ch));

    switch (ch)
    {
    case '=':
        if (peekChar() == '=')
        {
            readChar();
            tok = Token::Token(Token::TokenType::EQ, "==");
        }
        else
        {
            tok = Token::Token(Token::TokenType::ASSIGN, "=");
        }
        break;
    case '+':
        tok = Token::Token(Token::TokenType::PLUS, "+");
        break;
    case '-':
        tok = Token::Token(Token::TokenType::MINUS, "-");
        break;
    case '!':
        if (peekChar() == '=')
        {
            readChar();
            tok = Token::Token(Token::TokenType::NOT_EQ, "!=");
        }
        else
        {
            tok = Token::Token(Token::TokenType::BANG, "!");
        }
        break;
    case '*':
        tok = Token::Token(Token::TokenType::ASTERISK, "*");
        break;
    case '/':
        tok = Token::Token(Token::TokenType::SLASH, "/");
        break;
    case '<':
        tok = Token::Token(Token::TokenType::LT, "<");
        break;
    case '>':
        tok = Token::Token(Token::TokenType::GT, ">");
        break;
    case ';':
        tok = Token::Token(Token::TokenType::SEMICOLON, ";");
        break;
    case '(':
        tok = Token::Token(Token::TokenType::LPAREN, "(");
        break;
    case ')':
        tok = Token::Token(Token::TokenType::RPAREN, ")");
        break;
    case ',':
        tok = Token::Token(Token::TokenType::COMMA, ",");
        break;
    case '{':
        tok = Token::Token(Token::TokenType::LBRACE, "{");
        break;
    case '}':
        tok = Token::Token(Token::TokenType::RBRACE, "}");
        break;
    case 0:
        tok = Token::Token(Token::TokenType::EOF_, "");
        break;
    case '"':
        tok = Token::Token(Token::TokenType::STRING, readString());
        return tok;
    case '[':
        tok = Token::Token(Token::TokenType::LBRACKET, "[");
        break;
    case ']':
        tok = Token::Token(Token::TokenType::RBRACKET, "]");
        break;
    default:
        if (std::isalpha(ch) || ch == '_')
        {
            auto literal = readIdentifier();
            auto type = lookupIdent(literal);
            return {type, literal};
        }
        if (std::isdigit(ch))
        {
            return {Token::TokenType::INT, readNumber()};
        }
        break;
    }

    readChar();
    return tok;
}

} // namespace Lexer