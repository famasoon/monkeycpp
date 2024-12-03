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
    {Token::TokenType::LBRACKET, Precedence::INDEX},
};

Parser::Parser(std::unique_ptr<Lexer::Lexer> lexer)
    : lexer(std::move(lexer)), curToken(Token::TokenType::ILLEGAL, ""),
      peekToken(Token::TokenType::ILLEGAL, "")
{
    initParseFns();
    nextToken();
    nextToken();
}

void Parser::initParseFns()
{
    registerPrefix(Token::TokenType::IDENT, [this] { return parseIdentifier(); });
    registerPrefix(Token::TokenType::INT, std::bind(&Parser::parseIntegerLiteral, this));
    registerPrefix(Token::TokenType::BANG, std::bind(&Parser::parsePrefixExpression, this));
    registerPrefix(Token::TokenType::MINUS, std::bind(&Parser::parsePrefixExpression, this));
    registerPrefix(Token::TokenType::TRUE, std::bind(&Parser::parseBoolean, this));
    registerPrefix(Token::TokenType::FALSE, std::bind(&Parser::parseBoolean, this));
    registerPrefix(Token::TokenType::FUNCTION, [this] { return parseFunctionLiteral(); });
    registerPrefix(Token::TokenType::LPAREN, [this] { return parseGroupedExpression(); });
    registerPrefix(Token::TokenType::STRING, [this] { return parseStringLiteral(); });
    registerPrefix(Token::TokenType::LBRACKET, [this] { return parseArrayLiteral(); });
    registerPrefix(Token::TokenType::IF, [this] { return parseIfExpression(); });
    registerPrefix(Token::TokenType::WHILE, [this] { return parseWhileExpression(); });
    registerPrefix(Token::TokenType::FOR, [this] { return parseForExpression(); });
    registerPrefix(Token::TokenType::LET, [this] { return parseLetExpression(); });

    const std::vector<Token::TokenType> infixOps = {
        Token::TokenType::PLUS,     Token::TokenType::MINUS, Token::TokenType::SLASH,
        Token::TokenType::ASTERISK, Token::TokenType::EQ,    Token::TokenType::NOT_EQ,
        Token::TokenType::LT,       Token::TokenType::GT,    Token::TokenType::LBRACKET,
    };

    for (const auto &op : infixOps)
    {
        registerInfix(op, [this](auto left) { return parseInfixExpression(std::move(left)); });
    }

    registerInfix(Token::TokenType::LPAREN,
                  [this](auto left) { return parseCallExpression(std::move(left)); });
    registerInfix(Token::TokenType::LBRACKET,
                  [this](auto left) { return parseIndexExpression(std::move(left)); });
}

void Parser::registerPrefix(Token::TokenType type, PrefixParseFn fn)
{
    prefixParseFns[type] = std::move(fn);
}

void Parser::registerInfix(Token::TokenType type, InfixParseFn fn)
{
    infixParseFns[type] = std::move(fn);
}

void Parser::nextToken()
{
    curToken = peekToken;
    peekToken = lexer->NextToken();
}

bool Parser::curTokenIs(Token::TokenType t) const
{
    return curToken.getType() == t;
}
bool Parser::peekTokenIs(Token::TokenType t) const
{
    return peekToken.getType() == t;
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
    std::string msg = "expected next token to be " + Token::toString(t) + ", got " +
                      Token::toString(peekToken.getType()) + " instead";
    errors.push_back(msg);
}

void Parser::registerError(const std::string &msg)
{
    errors.push_back(msg);
}

void Parser::noPrefixParseFnError(Token::TokenType t)
{
    std::string msg = "no prefix parse function for " + Token::toString(t) + " found";
    errors.push_back(msg);
}

Parser::Precedence Parser::peekPrecedence() const
{
    auto it = precedences.find(peekToken.getType());
    if (it != precedences.end())
    {
        return it->second;
    }
    return Precedence::LOWEST;
}

Parser::Precedence Parser::curPrecedence() const
{
    auto it = precedences.find(curToken.getType());
    if (it != precedences.end())
    {
        return it->second;
    }
    return Precedence::LOWEST;
}

void Parser::trace(const std::string &msg)
{
    if (debugMode && debugOut)
    {
        *debugOut << getIndent() << msg << "\n";
    }
}

std::unique_ptr<AST::Program> Parser::ParseProgram()
{
    trace("START ParseProgram");
    increaseIndent();

    auto program = std::make_unique<AST::Program>();

    while (!curTokenIs(Token::TokenType::EOF_))
    {
        trace("Parsing statement: " + curToken.getLiteral());
        if (auto stmt = parseStatement())
        {
            program->statements.push_back(std::move(stmt));
            trace("Statement parsed successfully");
        }
        else
        {
            trace("Failed to parse statement");
        }
        nextToken();
    }

    decreaseIndent();
    trace("END ParseProgram");
    return program;
}

std::unique_ptr<AST::Statement> Parser::parseStatement()
{
    switch (curToken.getType())
    {
    case Token::TokenType::LET:
        return parseLetStatement();
    case Token::TokenType::RETURN:
        return parseReturnStatement();
    default:
        return parseExpressionStatement();
    }
}

std::unique_ptr<AST::Expression> Parser::parseExpression(Precedence precedence)
{
    trace("START parseExpression with precedence: " + std::to_string(static_cast<int>(precedence)));
    increaseIndent();

    auto prefix = prefixParseFns.find(curToken.getType());
    if (prefix == prefixParseFns.end())
    {
        noPrefixParseFnError(curToken.getType());
        trace("No prefix parse function found for: " + Token::toString(curToken.getType()));
        decreaseIndent();
        return nullptr;
    }

    auto leftExp = prefix->second();
    if (!leftExp)
    {
        trace("Failed to parse prefix expression");
        decreaseIndent();
        return nullptr;
    }

    while (!peekTokenIs(Token::TokenType::SEMICOLON) && precedence < peekPrecedence())
    {
        trace("Found infix operator: " + peekToken.getLiteral());
        auto infix = infixParseFns.find(peekToken.getType());
        if (infix == infixParseFns.end())
        {
            trace("No infix parse function found");
            break;
        }

        nextToken();
        leftExp = infix->second(std::move(leftExp));
        if (!leftExp)
        {
            trace("Failed to parse infix expression");
            break;
        }
    }

    decreaseIndent();
    trace("END parseExpression");
    return leftExp;
}

std::unique_ptr<AST::Expression> Parser::parseIdentifier()
{
    return std::make_unique<AST::Identifier>(curToken, curToken.getLiteral());
}

std::unique_ptr<AST::Expression> Parser::parseIntegerLiteral()
{
    try
    {
        int64_t value = std::stoll(curToken.getLiteral());
        return std::make_unique<AST::IntegerLiteral>(curToken, value);
    }
    catch (const std::exception &e)
    {
        registerError("could not parse " + curToken.getLiteral() + " as integer");
        return nullptr;
    }
}

std::unique_ptr<AST::Expression> Parser::parsePrefixExpression()
{
    auto expression = std::make_unique<AST::PrefixExpression>(curToken, curToken.getLiteral());
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
    auto expression =
        std::make_unique<AST::InfixExpression>(curToken, curToken.getLiteral(), std::move(left));

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

std::unique_ptr<AST::Expression> Parser::parseLetExpression() {
    auto stmt = parseLetStatement();
    if (!stmt) return nullptr;
    
    return std::make_unique<AST::LetExpression>(
        stmt->token,
        std::unique_ptr<AST::Identifier>(stmt->name.release()),
        std::unique_ptr<AST::Expression>(stmt->value.release())
    );
}

std::unique_ptr<AST::LetStatement> Parser::parseLetStatement()
{
    trace("START parseLetStatement");
    increaseIndent();

    auto stmt = std::make_unique<AST::LetStatement>(curToken);

    if (!expectPeek(Token::TokenType::IDENT))
    {
        decreaseIndent();
        return nullptr;
    }

    stmt->name = std::make_unique<AST::Identifier>(curToken, curToken.getLiteral());

    if (!expectPeek(Token::TokenType::ASSIGN))
    {
        expectAssignError();
        decreaseIndent();
        return nullptr;
    }

    nextToken();

    stmt->value = parseExpression(Precedence::LOWEST);
    if (!stmt->value)
    {
        decreaseIndent();
        return nullptr;
    }

    if (peekTokenIs(Token::TokenType::SEMICOLON))
    {
        nextToken();
    }

    decreaseIndent();
    trace("END parseLetStatement");
    return stmt;
}

std::unique_ptr<AST::ReturnStatement> Parser::parseReturnStatement()
{
    auto stmt = std::make_unique<AST::ReturnStatement>(curToken);
    nextToken();

    stmt->returnValue = parseExpression(Precedence::LOWEST);
    if (!stmt->returnValue)
    {
        registerError("Failed to parse return value");
        return nullptr;
    }

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
        return nullptr;

    if (peekTokenIs(Token::TokenType::SEMICOLON))
    {
        nextToken();
    }

    return stmt;
}

std::unique_ptr<AST::Expression> Parser::parseFunctionLiteral()
{
    trace("START parseFunctionLiteral");
    increaseIndent();

    auto lit = std::make_unique<AST::FunctionLiteral>(curToken);

    if (!expectPeek(Token::TokenType::LPAREN))
    {
        decreaseIndent();
        return nullptr;
    }

    lit->parameters = parseFunctionParameters();

    if (!expectPeek(Token::TokenType::LBRACE))
    {
        decreaseIndent();
        return nullptr;
    }

    lit->body = parseBlockStatement();
    if (!lit->body)
    {
        decreaseIndent();
        return nullptr;
    }

    decreaseIndent();
    trace("END parseFunctionLiteral");
    return lit;
}

std::vector<std::unique_ptr<AST::Identifier>> Parser::parseFunctionParameters()
{
    std::vector<std::unique_ptr<AST::Identifier>> params;

    if (peekTokenIs(Token::TokenType::RPAREN))
    {
        nextToken();
        return params;
    }

    nextToken();
    params.push_back(std::make_unique<AST::Identifier>(curToken, curToken.getLiteral()));

    while (peekTokenIs(Token::TokenType::COMMA))
    {
        nextToken(); // COMMA
        nextToken(); // パラメータ
        params.push_back(std::make_unique<AST::Identifier>(curToken, curToken.getLiteral()));
    }

    if (!expectPeek(Token::TokenType::RPAREN))
    {
        return std::vector<std::unique_ptr<AST::Identifier>>();
    }

    return params;
}

std::unique_ptr<AST::BlockStatement> Parser::parseBlockStatement()
{
    auto block = std::make_unique<AST::BlockStatement>(curToken);
    nextToken();

    while (!curTokenIs(Token::TokenType::RBRACE) && !curTokenIs(Token::TokenType::EOF_))
    {
        if (auto stmt = parseStatement())
        {
            block->statements.push_back(std::move(stmt));
        }
        nextToken();
    }

    if (!curTokenIs(Token::TokenType::RBRACE))
    {
        registerError("Expected '}' at the end of block statement");
        return nullptr;
    }

    return block;
}

std::unique_ptr<AST::Expression>
Parser::parseCallExpression(std::unique_ptr<AST::Expression> function)
{
    trace("START parseCallExpression");
    increaseIndent();

    auto exp = std::make_unique<AST::CallExpression>(curToken, std::move(function));
    exp->arguments = parseExpressionList(Token::TokenType::RPAREN);

    decreaseIndent();
    trace("END parseCallExpression");
    return exp;
}

std::vector<std::unique_ptr<AST::Expression>> Parser::parseExpressionList(Token::TokenType end)
{
    std::vector<std::unique_ptr<AST::Expression>> args;

    if (peekTokenIs(end))
    {
        nextToken();
        return args;
    }

    nextToken();
    args.push_back(parseExpression(Precedence::LOWEST));

    while (peekTokenIs(Token::TokenType::COMMA))
    {
        nextToken(); // consume comma
        nextToken();
        args.push_back(parseExpression(Precedence::LOWEST));
    }

    if (!expectPeek(end))
    {
        return std::vector<std::unique_ptr<AST::Expression>>();
    }

    return args;
}

std::unique_ptr<AST::Expression> Parser::parseBoolean()
{
    return std::make_unique<AST::BooleanLiteral>(curToken, curTokenIs(Token::TokenType::TRUE));
}

std::unique_ptr<AST::Expression> Parser::parseGroupedExpression()
{
    nextToken(); // '('をスキップ

    auto exp = parseExpression(Precedence::LOWEST);
    if (!expectPeek(Token::TokenType::RPAREN))
    {
        return nullptr;
    }

    return exp;
}

std::unique_ptr<AST::Expression> Parser::parseStringLiteral()
{
    return std::make_unique<AST::StringLiteral>(curToken, curToken.getLiteral());
}

std::unique_ptr<AST::Expression> Parser::parseArrayLiteral()
{
    trace("START parseArrayLiteral");
    increaseIndent();

    auto array = std::make_unique<AST::ArrayLiteral>(curToken);

    array->elements = parseExpressionList(Token::TokenType::RBRACKET);

    decreaseIndent();
    trace("END parseArrayLiteral");
    return array;
}

std::unique_ptr<AST::Expression> Parser::parseIndexExpression(std::unique_ptr<AST::Expression> left)
{
    trace("START parseIndexExpression");
    increaseIndent();

    auto expr = std::make_unique<AST::IndexExpression>(curToken, std::move(left));

    // '[' の次のトークンへ
    nextToken();

    expr->index = parseExpression(Precedence::LOWEST);
    if (!expr->index)
    {
        decreaseIndent();
        trace("Failed to parse index expression");
        return nullptr;
    }

    if (!expectPeek(Token::TokenType::RBRACKET))
    {
        decreaseIndent();
        trace("Expected ']'");
        return nullptr;
    }

    decreaseIndent();
    trace("END parseIndexExpression");
    return expr;
}

std::unique_ptr<AST::Expression> Parser::parseIfExpression()
{
    Token::Token ifToken = curToken;  // 'if'トークンを保存
    
    if (!expectPeek(Token::TokenType::LPAREN)) return nullptr;
    
    nextToken(); // '('を消費
    auto condition = parseExpression(Precedence::LOWEST);
    
    if (!expectPeek(Token::TokenType::RPAREN)) return nullptr;
    if (!expectPeek(Token::TokenType::LBRACE)) return nullptr;
    
    auto consequence = parseBlockStatement();
    
    std::unique_ptr<AST::BlockStatement> alternative;
    if (peekTokenIs(Token::TokenType::ELSE)) {
        nextToken();
        if (!expectPeek(Token::TokenType::LBRACE)) return nullptr;
        alternative = parseBlockStatement();
    }
    
    return std::make_unique<AST::IfExpression>(
        ifToken,
        std::move(condition), 
        std::move(consequence), 
        std::move(alternative)
    );
}

std::unique_ptr<AST::Expression> Parser::parseWhileExpression() {
    auto whileExpr = std::make_unique<AST::WhileExpression>(curToken);

    if (!expectPeek(Token::TokenType::LPAREN)) {
        return nullptr;
    }

    nextToken(); // skip '('
    whileExpr->condition = parseExpression(Precedence::LOWEST);

    if (!expectPeek(Token::TokenType::RPAREN)) {
        return nullptr;
    }

    if (!expectPeek(Token::TokenType::LBRACE)) {
        return nullptr;
    }

    whileExpr->body = parseBlockStatement();
    return whileExpr;
}

std::unique_ptr<AST::Expression> Parser::parseForExpression() {
    auto forExpr = std::make_unique<AST::ForExpression>(curToken);

    if (!expectPeek(Token::TokenType::LPAREN)) return nullptr;

    nextToken(); // skip '('
    if (curTokenIs(Token::TokenType::LET)) {
        auto letStmt = parseLetStatement();
        if (!letStmt) return nullptr;
        forExpr->init = std::make_unique<AST::LetExpression>(
            letStmt->token,
            std::unique_ptr<AST::Identifier>(letStmt->name.release()),
            std::unique_ptr<AST::Expression>(letStmt->value.release())
        );
    } else {
        forExpr->init = parseExpression(Precedence::LOWEST);
        if (!expectPeek(Token::TokenType::SEMICOLON)) return nullptr;
    }

    nextToken(); // skip ';'
    forExpr->condition = parseExpression(Precedence::LOWEST);

    if (!expectPeek(Token::TokenType::SEMICOLON)) return nullptr;

    nextToken(); // skip ';'
    if (curTokenIs(Token::TokenType::LET)) {
        auto letStmt = parseLetStatement();
        if (!letStmt) return nullptr;
        forExpr->update = std::make_unique<AST::LetExpression>(
            letStmt->token,
            std::unique_ptr<AST::Identifier>(letStmt->name.release()),
            std::unique_ptr<AST::Expression>(letStmt->value.release())
        );
    } else {
        forExpr->update = parseExpression(Precedence::LOWEST);
    }

    if (!expectPeek(Token::TokenType::RPAREN)) return nullptr;
    if (!expectPeek(Token::TokenType::LBRACE)) return nullptr;

    forExpr->body = parseBlockStatement();
    return forExpr;
}

// 代入演算子のエラーメッセージを追加
void Parser::expectAssignError()
{
    std::string msg = "expected next token to be ASSIGN";
    errors.push_back(msg);
}

} // namespace Parser
