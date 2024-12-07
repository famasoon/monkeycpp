#include "ast.hpp"

namespace AST
{
// Program実装
std::string Program::TokenLiteral() const
{
    if (!statements.empty() && statements[0])
    {
        return statements[0]->TokenLiteral();
    }
    return "";
}

std::string Program::String() const
{
    std::string out;
    for (const auto &stmt : statements)
    {
        if (stmt)
        {
            out += stmt->String();
        }
    }
    return out;
}

// BlockStatement implementation
BlockStatement::BlockStatement(Token::Token token) : token(std::move(token))
{
}

void BlockStatement::statementNode()
{
}

std::string BlockStatement::TokenLiteral() const
{
    return token.getLiteral();
}

std::string BlockStatement::String() const
{
    std::string out = "{ ";
    for (const auto &stmt : statements)
    {
        if (stmt)
        {
            out += stmt->String();
        }
    }
    out += " }";
    return out;
}

// Identifier implementation
Identifier::Identifier(Token::Token token, std::string value)
    : token(std::move(token)), value(std::move(value))
{
}

void Identifier::expressionNode()
{
}

std::string Identifier::TokenLiteral() const
{
    return token.getLiteral();
}

std::string Identifier::String() const
{
    return value;
}

Expression *Identifier::clone() const
{
    return new Identifier(token, value);
}

// LetStatement implementation
LetStatement::LetStatement(Token::Token token) : token(std::move(token))
{
}

void LetStatement::statementNode()
{
}

std::string LetStatement::TokenLiteral() const
{
    return token.getLiteral();
}

std::string LetStatement::String() const
{
    std::string out = TokenLiteral() + " ";
    if (name)
    {
        out += name->String();
    }
    out += " = ";
    if (value)
    {
        out += value->String();
    }
    out += ";";
    return out;
}

Statement *LetStatement::clone() const
{
    auto cloned = new LetStatement(token);
    if (name)
    {
        cloned->name.reset(static_cast<Identifier *>(name->clone()));
    }
    if (value)
    {
        cloned->value.reset(value->clone());
    }
    return cloned;
}

// ReturnStatement implementation
ReturnStatement::ReturnStatement(Token::Token token) : token(std::move(token))
{
}

void ReturnStatement::statementNode()
{
}

std::string ReturnStatement::TokenLiteral() const
{
    return token.getLiteral();
}

std::string ReturnStatement::String() const
{
    std::string out = TokenLiteral() + " ";
    if (returnValue)
    {
        out += returnValue->String();
    }
    out += ";";
    return out;
}

Statement *ReturnStatement::clone() const
{
    auto cloned = new ReturnStatement(token);
    if (returnValue)
    {
        cloned->returnValue.reset(returnValue->clone());
    }
    return cloned;
}

// ExpressionStatement implementation
ExpressionStatement::ExpressionStatement(Token::Token token) : token(std::move(token))
{
}

void ExpressionStatement::statementNode()
{
}

std::string ExpressionStatement::TokenLiteral() const
{
    return token.getLiteral();
}

std::string ExpressionStatement::String() const
{
    return expression ? expression->String() : "";
}

// IntegerLiteral implementation
IntegerLiteral::IntegerLiteral(Token::Token token, int64_t value)
    : token(std::move(token)), value(value)
{
}

void IntegerLiteral::expressionNode()
{
}

std::string IntegerLiteral::TokenLiteral() const
{
    return token.getLiteral();
}

std::string IntegerLiteral::String() const
{
    return token.getLiteral();
}

Expression *IntegerLiteral::clone() const
{
    return new IntegerLiteral(token, value);
}

// PrefixExpression implementation
PrefixExpression::PrefixExpression(Token::Token token, std::string op)
    : token(std::move(token)), op(std::move(op))
{
}

void PrefixExpression::expressionNode()
{
}

std::string PrefixExpression::TokenLiteral() const
{
    return token.getLiteral();
}

std::string PrefixExpression::String() const
{
    std::string result = "(";
    result += op;
    if (right)
    {
        result += right->String();
    }
    result += ")";
    return result;
}

Expression *PrefixExpression::clone() const
{
    auto cloned = new PrefixExpression(token, op);
    if (right)
    {
        cloned->right.reset(right->clone());
    }
    return cloned;
}

// InfixExpression implementation
InfixExpression::InfixExpression(Token::Token token, std::string op,
                                 std::unique_ptr<Expression> left)
    : token(std::move(token)), op(std::move(op)), left(std::move(left))
{
}

void InfixExpression::expressionNode()
{
}

std::string InfixExpression::TokenLiteral() const
{
    return token.getLiteral();
}

std::string InfixExpression::String() const
{
    std::string result = "(";
    if (left)
    {
        result += left->String();
    }
    result += " " + op + " ";
    if (right)
    {
        result += right->String();
    }
    result += ")";
    return result;
}

Expression *InfixExpression::clone() const
{
    auto cloned = new InfixExpression(token, op, nullptr);
    if (left)
    {
        cloned->left.reset(left->clone());
    }
    if (right)
    {
        cloned->right.reset(right->clone());
    }
    return cloned;
}

// BooleanLiteral implementation
BooleanLiteral::BooleanLiteral(Token::Token token, bool value)
    : token(std::move(token)), value(value)
{
}

void BooleanLiteral::expressionNode()
{
}

std::string BooleanLiteral::TokenLiteral() const
{
    return token.getLiteral();
}

std::string BooleanLiteral::String() const
{
    return token.getLiteral();
}

Expression *BooleanLiteral::clone() const
{
    return new BooleanLiteral(token, value);
}

// FunctionLiteral implementation
FunctionLiteral::FunctionLiteral(Token::Token token) : token(std::move(token))
{
}

void FunctionLiteral::expressionNode()
{
}

std::string FunctionLiteral::TokenLiteral() const
{
    return token.getLiteral();
}

std::string FunctionLiteral::String() const
{
    std::string out = token.getLiteral() + "(";
    for (size_t i = 0; i < parameters.size(); i++)
    {
        out += parameters[i]->String();
        if (i < parameters.size() - 1)
        {
            out += ", ";
        }
    }
    out += ") ";
    if (body)
    {
        out += body->String();
    }
    return out;
}

Expression *FunctionLiteral::clone() const
{
    auto cloned = new FunctionLiteral(token);
    for (const auto &param : parameters)
    {
        if (param)
        {
            cloned->parameters.push_back(
                std::unique_ptr<Identifier>(static_cast<Identifier *>(param->clone())));
        }
    }
    if (body)
    {
        cloned->body.reset(static_cast<BlockStatement *>(body->clone()));
    }
    return cloned;
}

// CallExpression implementation
CallExpression::CallExpression(Token::Token token, std::unique_ptr<Expression> function)
    : token(std::move(token)), function(std::move(function))
{
}

void CallExpression::expressionNode()
{
}

std::string CallExpression::TokenLiteral() const
{
    return token.getLiteral();
}

std::string CallExpression::String() const
{
    std::string out = function->String() + "(";
    for (size_t i = 0; i < arguments.size(); i++)
    {
        out += arguments[i]->String();
        if (i < arguments.size() - 1)
        {
            out += ", ";
        }
    }
    out += ")";
    return out;
}

Expression *CallExpression::clone() const
{
    auto cloned = new CallExpression(token, nullptr);
    if (function)
    {
        cloned->function.reset(function->clone());
    }
    for (const auto &arg : arguments)
    {
        if (arg)
        {
            cloned->arguments.push_back(std::unique_ptr<Expression>(arg->clone()));
        }
    }
    return cloned;
}

// StringLiteral implementation
StringLiteral::StringLiteral(Token::Token token, std::string value)
    : token(std::move(token)), value(std::move(value))
{
}

void StringLiteral::expressionNode()
{
}

std::string StringLiteral::TokenLiteral() const
{
    return token.getLiteral();
}

std::string StringLiteral::String() const
{
    return "\"" + value + "\"";
}

Expression *StringLiteral::clone() const
{
    return new StringLiteral(token, value);
}

// ArrayLiteral実装
ArrayLiteral::ArrayLiteral(Token::Token token) : token(std::move(token))
{
}

void ArrayLiteral::expressionNode()
{
}

std::string ArrayLiteral::TokenLiteral() const
{
    return token.getLiteral();
}

std::string ArrayLiteral::String() const
{
    std::string out = "[";
    for (size_t i = 0; i < elements.size(); ++i)
    {
        if (elements[i])
        {
            out += elements[i]->String();
        }
        if (i < elements.size() - 1)
        {
            out += ", ";
        }
    }
    out += "]";
    return out;
}

Expression *ArrayLiteral::clone() const
{
    auto cloned = new ArrayLiteral(token);
    for (const auto &elem : elements)
    {
        if (elem)
        {
            cloned->elements.push_back(std::unique_ptr<Expression>(elem->clone()));
        }
    }
    return cloned;
}

// IndexExpression実装
IndexExpression::IndexExpression(Token::Token token, std::unique_ptr<Expression> left)
    : token(std::move(token)), left(std::move(left))
{
}

void IndexExpression::expressionNode()
{
}

std::string IndexExpression::TokenLiteral() const
{
    return token.getLiteral();
}

std::string IndexExpression::String() const
{
    std::string out;
    if (left)
    {
        out += "(" + left->String();
    }
    out += "[";
    if (index)
    {
        out += index->String();
    }
    out += "])";
    return out;
}

Expression *IndexExpression::clone() const
{
    auto cloned = new IndexExpression(token, nullptr);
    if (left)
    {
        cloned->left.reset(left->clone());
    }
    if (index)
    {
        cloned->index.reset(index->clone());
    }
    return cloned;
}

// HashLiteral実装
HashLiteral::HashLiteral(Token::Token tok) : token(std::move(tok))
{
}

void HashLiteral::expressionNode()
{
}

std::string HashLiteral::TokenLiteral() const
{
    return token.getLiteral();
}

std::string HashLiteral::String() const
{
    std::string result = "{";
    bool first = true;

    for (const auto &pair : pairs)
    {
        if (!first)
        {
            result += ", ";
        }
        first = false;

        if (pair.first)
        {
            result += pair.first->String();
        }
        result += ": ";
        if (pair.second)
        {
            result += pair.second->String();
        }
    }

    result += "}";
    return result;
}

Expression *HashLiteral::clone() const
{
    auto cloned = new HashLiteral(token);
    for (const auto &[key, value] : pairs)
    {
        if (key && value)
        {
            auto clonedKey = std::unique_ptr<Expression>(key->clone());
            auto clonedValue = std::unique_ptr<Expression>(value->clone());
            cloned->pairs[std::move(clonedKey)] = std::move(clonedValue);
        }
    }
    return cloned;
}

// ExpressionStatementのコピーコンストラクタ実装
ExpressionStatement::ExpressionStatement(const ExpressionStatement &other)
    : token(other.token),
      expression(other.expression ? std::unique_ptr<Expression>(other.expression->clone())
                                  : nullptr)
{
}

// BlockStatementのコピーコンストラクタ実装
BlockStatement::BlockStatement(const BlockStatement &other) : token(other.token)
{
    statements.reserve(other.statements.size());
    for (const auto &stmt : other.statements)
    {
        if (stmt)
        {
            statements.push_back(std::unique_ptr<Statement>(stmt->clone()));
        }
    }
}

// ExpressionStatementのclone実装
Statement *ExpressionStatement::clone() const
{
    auto cloned = new ExpressionStatement(token);
    if (expression)
    {
        cloned->expression.reset(expression->clone());
    }
    return cloned;
}

// BlockStatementのclone実装
Statement *BlockStatement::clone() const
{
    auto cloned = new BlockStatement(token);
    cloned->statements.reserve(statements.size());
    for (const auto &stmt : statements)
    {
        if (stmt)
        {
            cloned->statements.push_back(std::unique_ptr<Statement>(stmt->clone()));
        }
    }
    return cloned;
}

// Program classのメモリ管理改善
void Program::clearStatements()
{
    statements.clear();
}

// 新しいメソッドの追加
void Program::addStatement(std::unique_ptr<Statement> stmt)
{
    if (stmt)
    {
        statements.push_back(std::move(stmt));
    }
}

WhileExpression::WhileExpression(Token::Token tok) 
    : token(std::move(tok)) {}

WhileExpression::WhileExpression(Token::Token tok,
                                std::unique_ptr<Expression> cond,
                                std::unique_ptr<BlockStatement> b)
    : token(std::move(tok))
    , condition(std::move(cond))
    , body(std::move(b)) {}

void WhileExpression::expressionNode() {}

std::string WhileExpression::TokenLiteral() const { 
    return token.getLiteral(); 
}

std::string WhileExpression::String() const {
    std::string out = "while (" + (condition ? condition->String() : "") + ") ";
    if (body) {
        out += body->String();
    }
    return out;
}

Expression* WhileExpression::clone() const {
    return new WhileExpression(
        token,
        condition ? std::unique_ptr<Expression>(condition->clone()) : nullptr,
        body ? std::unique_ptr<BlockStatement>(static_cast<BlockStatement*>(body->clone())) : nullptr
    );
}

ForExpression::ForExpression(Token::Token tok)
    : token(std::move(tok)) {}

ForExpression::ForExpression(Token::Token tok,
                           std::unique_ptr<Expression> init,
                           std::unique_ptr<Expression> cond,
                           std::unique_ptr<Expression> update,
                           std::unique_ptr<BlockStatement> b)
    : token(std::move(tok))
    , init(std::move(init))
    , condition(std::move(cond))
    , update(std::move(update))
    , body(std::move(b)) {}

void ForExpression::expressionNode() {}

std::string ForExpression::TokenLiteral() const {
    return token.getLiteral();
}

std::string ForExpression::String() const {
    std::string out = "for (";
    if (init) out += init->String();
    out += "; ";
    if (condition) out += condition->String();
    out += "; ";
    if (update) out += update->String();
    out += ") ";
    if (body) out += body->String();
    return out;
}

Expression* ForExpression::clone() const {
    return new ForExpression(
        token,
        init ? std::unique_ptr<Expression>(init->clone()) : nullptr,
        condition ? std::unique_ptr<Expression>(condition->clone()) : nullptr,
        update ? std::unique_ptr<Expression>(update->clone()) : nullptr,
        body ? std::unique_ptr<BlockStatement>(static_cast<BlockStatement*>(body->clone())) : nullptr
    );
}

LetExpression::LetExpression(Token::Token tok,
                           std::unique_ptr<Identifier> name,
                           std::unique_ptr<Expression> value)
    : token(std::move(tok))
    , name(std::move(name))
    , value(std::move(value)) {}

void LetExpression::expressionNode() {}

std::string LetExpression::TokenLiteral() const {
    return token.getLiteral();
}

std::string LetExpression::String() const {
    std::string out = "let " + name->String() + " = ";
    if (value) out += value->String();
    return out;
}

Expression* LetExpression::clone() const {
    return new LetExpression(
        token,
        std::unique_ptr<Identifier>(static_cast<Identifier*>(name->clone())),
        value ? std::unique_ptr<Expression>(value->clone()) : nullptr
    );
}

// IfExpression implementation
IfExpression::IfExpression(Token::Token tok,
                          std::unique_ptr<Expression> cond,
                          std::unique_ptr<BlockStatement> cons,
                          std::unique_ptr<BlockStatement> alt)
    : token(std::move(tok))
    , condition(std::move(cond))
    , consequence(std::move(cons))
    , alternative(std::move(alt)) {}

void IfExpression::expressionNode() {}

std::string IfExpression::TokenLiteral() const {
    return token.getLiteral();
}

std::string IfExpression::String() const {
    std::string result = "if" + condition->String() + " " + consequence->String();
    if (alternative) {
        result += "else " + alternative->String();
    }
    return result;
}

Expression* IfExpression::clone() const {
    return new IfExpression(
        token,
        std::unique_ptr<Expression>(condition->clone()),
        std::unique_ptr<BlockStatement>(static_cast<BlockStatement*>(consequence->clone())),
        alternative ? std::unique_ptr<BlockStatement>(static_cast<BlockStatement*>(alternative->clone())) : nullptr
    );
}
} // namespace AST
