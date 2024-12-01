#include "parser.hpp"

namespace Parser
{

  std::unordered_map<Token::TokenType, Parser::Precedence> Parser::precedences = {
      {Token::TokenType::EQ, Precedence::EQUALS},
      {Token::TokenType::NOT_EQ, Precedence::EQUALS},
      {Token::TokenType::LT, Precedence::LESSGREATER},
      {Token::TokenType::GT, Precedence::LESSGREATER},
      {Token::TokenType::PLUS, Precedence::SUM},
      {Token::TokenType::MINUS, Precedence::SUM},
      {Token::TokenType::SLASH, Precedence::PRODUCT},
      {Token::TokenType::ASTERISK, Precedence::PRODUCT},
      {Token::TokenType::LPAREN, Precedence::CALL},
  };

  Parser::Parser(Lexer::Lexer lexer) : lexer(std::move(lexer)),
                                       curToken(Token::TokenType::ILLEGAL, ""),
                                       peekToken(Token::TokenType::ILLEGAL, "")
  {
    // パース関数の登録
    registerPrefix(Token::TokenType::IDENT, [this]()
                   { return parseIdentifier(); });
    registerPrefix(Token::TokenType::INT, [this]()
                   { return parseIntegerLiteral(); });
    registerPrefix(Token::TokenType::BANG, [this]()
                   { return parsePrefixExpression(); });
    registerPrefix(Token::TokenType::MINUS, [this]()
                   { return parsePrefixExpression(); });

    registerInfix(Token::TokenType::PLUS, [this](auto left)
                  { return parseInfixExpression(std::move(left)); });
    registerInfix(Token::TokenType::MINUS, [this](auto left)
                  { return parseInfixExpression(std::move(left)); });
    registerInfix(Token::TokenType::SLASH, [this](auto left)
                  { return parseInfixExpression(std::move(left)); });
    registerInfix(Token::TokenType::ASTERISK, [this](auto left)
                  { return parseInfixExpression(std::move(left)); });
    registerInfix(Token::TokenType::EQ, [this](auto left)
                  { return parseInfixExpression(std::move(left)); });
    registerInfix(Token::TokenType::NOT_EQ, [this](auto left)
                  { return parseInfixExpression(std::move(left)); });
    registerInfix(Token::TokenType::LT, [this](auto left)
                  { return parseInfixExpression(std::move(left)); });
    registerInfix(Token::TokenType::GT, [this](auto left)
                  { return parseInfixExpression(std::move(left)); });

    // 2つのトークンを読み込む
    nextToken();
    nextToken();
  }

  void Parser::registerPrefix(Token::TokenType type, PrefixParseFn fn)
  {
    prefixParseFns[type] = std::move(fn);
  }

  void Parser::registerInfix(Token::TokenType type, InfixParseFn fn)
  {
    infixParseFns[type] = std::move(fn);
  }

  Parser::Precedence Parser::peekPrecedence() const
  {
    auto it = precedences.find(peekToken.type);
    if (it != precedences.end())
    {
      return it->second;
    }
    return Precedence::LOWEST;
  }

  Parser::Precedence Parser::curPrecedence() const
  {
    auto it = precedences.find(curToken.type);
    if (it != precedences.end())
    {
      return it->second;
    }
    return Precedence::LOWEST;
  }

  std::unique_ptr<AST::Expression> Parser::parseExpression(Precedence precedence)
  {
    auto prefix = prefixParseFns.find(curToken.type);
    if (prefix == prefixParseFns.end())
    {
      noPrefixParseFnError(curToken.type);
      return nullptr;
    }

    auto leftExp = prefix->second();
    if (!leftExp)
    {
      return nullptr;
    }

    while (!peekTokenIs(Token::TokenType::SEMICOLON) && precedence < peekPrecedence())
    {
      auto infix = infixParseFns.find(peekToken.type);
      if (infix == infixParseFns.end())
      {
        return leftExp;
      }

      nextToken();
      leftExp = infix->second(std::move(leftExp));
    }

    return leftExp;
  }

  std::unique_ptr<AST::Expression> Parser::parseIntegerLiteral()
  {
    try
    {
      int64_t value = std::stoll(curToken.literal);
      return std::make_unique<AST::IntegerLiteral>(curToken, value);
    }
    catch (const std::exception &e)
    {
      std::string msg = "could not parse " + curToken.literal + " as integer";
      errors.push_back(msg);
      return nullptr;
    }
  }

  std::unique_ptr<AST::Expression> Parser::parsePrefixExpression()
  {
    auto expression = std::make_unique<AST::PrefixExpression>(
        curToken,
        curToken.literal);

    nextToken();

    expression->right = parseExpression(Precedence::PREFIX);
    if (!expression->right)
    {
      registerError("Could not parse expression for prefix operator");
      return nullptr;
    }

    return expression;
  }

  std::unique_ptr<AST::Expression> Parser::parseInfixExpression(std::unique_ptr<AST::Expression> left)
  {
    auto expression = std::make_unique<AST::InfixExpression>(
        curToken,
        curToken.literal,
        std::move(left));

    auto precedence = curPrecedence();
    nextToken();

    expression->right = parseExpression(precedence);
    if (!expression->right)
    {
      registerError("Could not parse right side of infix expression");
      return nullptr;
    }

    return expression;
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

    nextToken(); // ASSIGN の次のトークンに移動

    // 式をパースする
    stmt->value = parseExpression(Precedence::LOWEST);
    if (!stmt->value)
    {
      return nullptr;
    }

    if (!peekTokenIs(Token::TokenType::SEMICOLON))
    {
      registerError("Expected semicolon after let statement");
      return nullptr;
    }
    nextToken();

    return stmt;
  }

  std::unique_ptr<AST::ReturnStatement> Parser::parseReturnStatement()
  {
    auto stmt = std::make_unique<AST::ReturnStatement>(curToken);

    nextToken();

    // 式をパースする
    stmt->returnValue = parseExpression(Precedence::LOWEST);
    if (!stmt->returnValue)
    {
      registerError("Failed to parse return value");
      return nullptr;
    }

    // セミコロンは省略可���
    if (peekTokenIs(Token::TokenType::SEMICOLON))
    {
      nextToken();
    }

    return stmt;
  }

  std::unique_ptr<AST::ExpressionStatement> Parser::parseExpressionStatement()
  {
    auto stmt = std::make_unique<AST::ExpressionStatement>(curToken);

    stmt->expression = parseExpression(Precedence::LOWEST);
    if (!stmt->expression)
    {
      return nullptr;
    }

    // セミコロンは省略可能
    if (peekTokenIs(Token::TokenType::SEMICOLON))
    {
      nextToken();
    }

    return stmt;
  }

  std::unique_ptr<AST::Expression> Parser::parseIdentifier()
  {
    return std::make_unique<AST::Identifier>(curToken, curToken.literal);
  }

  void Parser::noPrefixParseFnError(Token::TokenType t)
  {
    std::string msg = "no prefix parse function for " + Token::toString(t) + " found";
    errors.push_back(msg);
  }

} // namespace Parser
