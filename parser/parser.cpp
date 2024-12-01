#include "parser.hpp"

namespace Parser
{

  Parser::Parser(Lexer::Lexer lexer) : lexer(std::move(lexer)),
                                       curToken(Token::TokenType::ILLEGAL, ""),
                                       peekToken(Token::TokenType::ILLEGAL, "")
  {
    // 2つのトークンを読み込む。curTokenとpeekTokenの両方をセットする
    nextToken();
    nextToken();
  }

  void Parser::nextToken()
  {
    curToken = peekToken;
    peekToken = lexer.NextToken();
  }

  bool Parser::curTokenIs(Token::TokenType t) const
  {
    return curToken.type == t;
  }

  bool Parser::peekTokenIs(Token::TokenType t) const
  {
    return peekToken.type == t;
  }

  bool Parser::expectPeek(Token::TokenType t)
  {
    if (peekTokenIs(t))
    {
      nextToken();
      return true;
    }
    peekError(t);
    return false;
  }

  void Parser::peekError(Token::TokenType t)
  {
    std::string msg = "expected next token to be " + Token::toString(t) +
                      ", got " + Token::toString(peekToken.type) + " instead";
    errors.push_back(msg);
  }

  void Parser::registerError(const std::string &msg)
  {
    errors.push_back(msg);
  }

  std::unique_ptr<AST::Program> Parser::ParseProgram()
  {
    auto program = std::make_unique<AST::Program>();

    while (curToken.type != Token::TokenType::EOF_)
    {
      auto stmt = parseStatement();
      if (stmt)
      {
        program->statements.push_back(std::move(stmt));
      }
      nextToken();
    }

    return program;
  }

  std::unique_ptr<AST::Statement> Parser::parseStatement()
  {
    switch (curToken.type)
    {
    case Token::TokenType::LET:
      return parseLetStatement();
    case Token::TokenType::RETURN:
      return parseReturnStatement();
    default:
      return parseExpressionStatement();
    }
  }

  std::unique_ptr<AST::LetStatement> Parser::parseLetStatement()
  {
    auto stmt = std::make_unique<AST::LetStatement>(curToken);

    if (!expectPeek(Token::TokenType::IDENT))
    {
      return nullptr;
    }

    stmt->name = std::make_unique<AST::Identifier>(curToken, curToken.literal);

    if (!expectPeek(Token::TokenType::ASSIGN))
    {
      return nullptr;
    }

    // TODO: セミコロンまでの式をスキップする
    while (!curTokenIs(Token::TokenType::SEMICOLON))
    {
      nextToken();
    }

    return stmt;
  }

  std::unique_ptr<AST::ReturnStatement> Parser::parseReturnStatement()
  {
    auto stmt = std::make_unique<AST::ReturnStatement>(curToken);

    nextToken();

    // TODO: セミコロンまでの式をスキップする
    while (!curTokenIs(Token::TokenType::SEMICOLON))
    {
      nextToken();
    }

    return stmt;
  }

  std::unique_ptr<AST::ExpressionStatement> Parser::parseExpressionStatement()
  {
    auto stmt = std::make_unique<AST::ExpressionStatement>(curToken);

    stmt->expression = parseExpression(Precedence::LOWEST);

    if (peekTokenIs(Token::TokenType::SEMICOLON))
    {
      nextToken();
    }

    return stmt;
  }

  std::unique_ptr<AST::Expression> Parser::parseExpression([[maybe_unused]] Precedence precedence)
  {
    // 現時点では識別子のみをパースする
    switch (curToken.type)
    {
    case Token::TokenType::IDENT:
      return parseIdentifier();
    default:
      return nullptr;
    }
  }

  std::unique_ptr<AST::Expression> Parser::parseIdentifier()
  {
    return std::make_unique<AST::Identifier>(curToken, curToken.literal);
  }

} // namespace Parser
