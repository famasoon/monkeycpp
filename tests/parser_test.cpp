#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
#include <algorithm>
#include <gtest/gtest.h>

namespace
{
// ヘルパー関数
struct ParseResult
{
    std::unique_ptr<AST::Program> program;
    std::unique_ptr<Parser::Parser> parser_owner;
    const Parser::Parser *parser;

    ParseResult(std::unique_ptr<AST::Program> p, std::unique_ptr<Parser::Parser> po)
        : program(std::move(p)), parser_owner(std::move(po)), parser(parser_owner.get())
    {
    }
};

ParseResult ParseInput(const std::string &input)
{
    Lexer::Lexer lexer(input);
    auto parser =
        std::make_unique<Parser::Parser>(std::make_unique<Lexer::Lexer>(std::move(lexer)));
    auto parser_ptr = parser.get();
    auto program = parser_ptr->ParseProgram();
    return ParseResult(std::move(program), std::move(parser));
}

void CheckParserErrors(const Parser::Parser &p)
{
    const auto &errors = p.Errors();
    if (!errors.empty())
    {
        std::string errorMsg = "Parser has " + std::to_string(errors.size()) + " errors:\n";
        for (const auto &err : errors)
        {
            errorMsg += "parser error: " + err + "\n";
        }
        FAIL() << errorMsg;
    }
}

bool ContainsError(const std::vector<std::string> &errors, const std::string &substring)
{
    return std::any_of(errors.begin(), errors.end(), [&](const std::string &error)
                       { return error.find(substring) != std::string::npos; });
}

// テストケース用の構造体
struct LetStatementTestCase
{
    std::string input;
    std::string expectedIdentifier;
};

struct OperatorPrecedenceTest
{
    std::string input;
    std::string expected;
};

struct PrefixTest
{
    std::string input;
    std::string op;
    int64_t value;
};

struct InfixTest
{
    std::string input;
    int64_t leftValue;
    std::string op;
    int64_t rightValue;
};
} // namespace

class ParserTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
    }
    void TearDown() override
    {
    }
};

// Let文のテスト
TEST_F(ParserTest, TestLetStatements)
{
    std::string input = R"(
        let x = 5;
        let y = 10;
        let foobar = 838383;
    )";

    Lexer::Lexer lexer(input);
    Parser::Parser parser(std::make_unique<Lexer::Lexer>(std::move(lexer)));

    auto program = parser.ParseProgram();
    CheckParserErrors(parser);

    ASSERT_NE(program, nullptr) << "ParseProgram() returned nullptr";
    CheckParserErrors(parser);
    ASSERT_EQ(program->statements.size(), 3);

    auto stmt1 = dynamic_cast<AST::LetStatement *>(program->statements[0].get());
    ASSERT_NE(stmt1, nullptr) << "Not a LetStatement";
    EXPECT_EQ(stmt1->TokenLiteral(), "let");
    EXPECT_EQ(stmt1->name->value, "x");
    EXPECT_EQ(stmt1->name->TokenLiteral(), "x");

    auto stmt2 = dynamic_cast<AST::LetStatement *>(program->statements[1].get());
    ASSERT_NE(stmt2, nullptr) << "Not a LetStatement";
    EXPECT_EQ(stmt2->TokenLiteral(), "let");
    EXPECT_EQ(stmt2->name->value, "y");
    EXPECT_EQ(stmt2->name->TokenLiteral(), "y");

    auto stmt3 = dynamic_cast<AST::LetStatement *>(program->statements[2].get());
    ASSERT_NE(stmt3, nullptr) << "Not a LetStatement";
    EXPECT_EQ(stmt3->TokenLiteral(), "let");
    EXPECT_EQ(stmt3->name->value, "foobar");
    EXPECT_EQ(stmt3->name->TokenLiteral(), "foobar");
}

// エラー処理のテスト（修正版）
TEST_F(ParserTest, TestLetStatementErrors)
{
    const std::string input = R"(
        let = 5;
        let x 5;
        let 838383;
    )";

    auto [program, parser_owner, parser] = ParseInput(input);
    const auto &errors = parser->Errors();

    ASSERT_GE(errors.size(), 3);
    EXPECT_TRUE(ContainsError(errors, "IDENT"));
    EXPECT_TRUE(ContainsError(errors, "ASSIGN"));
}

// 演算子の優先順位テスト
TEST_F(ParserTest, TestOperatorPrecedenceParsing)
{
    std::vector<OperatorPrecedenceTest> tests = {
        {"-a * b", "((-a) * b)"},
        {"!-a", "(!(-a))"},
        {"a + b + c", "((a + b) + c)"},
        {"a + b - c", "((a + b) - c)"},
        {"a * b * c", "((a * b) * c)"},
        {"a * b / c", "((a * b) / c)"},
        {"a + b / c", "(a + (b / c))"},
        {"a + b * c + d / e - f", "(((a + (b * c)) + (d / e)) - f)"},
        {"3 + 4; -5 * 5", "(3 + 4)((-5) * 5)"},
        {"5 > 4 == 3 < 4", "((5 > 4) == (3 < 4))"},
        {"5 < 4 != 3 > 4", "((5 < 4) != (3 > 4))"},
        {"3 + 4 * 5 == 3 * 1 + 4 * 5", "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))"}};

    for (const auto &tt : tests)
    {
        auto [program, parser_owner, parser] = ParseInput(tt.input);
        ASSERT_NE(program, nullptr);
        ASSERT_TRUE(parser->Errors().empty()) << "Parser errors: " << parser->Errors()[0];

        auto actual = program->String();
        actual.erase(std::remove_if(actual.begin(), actual.end(),
                                    [](char c) { return c == ' ' || c == ';'; }),
                     actual.end());

        std::string expected = tt.expected;
        expected.erase(std::remove(expected.begin(), expected.end(), ' '), expected.end());

        EXPECT_EQ(actual, expected)
            << "Input: " << tt.input << "\nExpected: " << tt.expected << "\nGot: " << actual;
    }
}

// 式のテスト
TEST_F(ParserTest, TestPrefixExpressions)
{
    std::vector<PrefixTest> tests = {
        {"!5;", "!", 5},
        {"-15;", "-", 15},
    };

    for (const auto &tt : tests)
    {
        auto [program, parser_owner, parser] = ParseInput(tt.input);
        ASSERT_NE(program.get(), nullptr);
        ASSERT_TRUE(parser->Errors().empty()) << "Parser errors: " << parser->Errors()[0];
        ASSERT_EQ(program->statements.size(), 1);

        auto stmt = dynamic_cast<AST::ExpressionStatement *>(program->statements[0].get());
        ASSERT_NE(stmt, nullptr);
        auto exp = dynamic_cast<AST::PrefixExpression *>(stmt->expression.get());
        ASSERT_NE(exp, nullptr);

        EXPECT_EQ(exp->op, tt.op);
        auto right = dynamic_cast<AST::IntegerLiteral *>(exp->right.get());
        ASSERT_NE(right, nullptr);
        EXPECT_EQ(right->value, tt.value);
    }
}

// ... 他のテストケースも同様にリファクタリング ...

TEST_F(ParserTest, TestFunctionLiteral)
{
    const std::string input = "fn(x, y) { x + y; }";

    auto [program, parser_owner, parser] = ParseInput(input);
    ASSERT_NE(program.get(), nullptr);
    ASSERT_TRUE(parser->Errors().empty()) << "Parser errors: " << parser->Errors()[0];
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<AST::ExpressionStatement *>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);

    auto func = dynamic_cast<AST::FunctionLiteral *>(stmt->expression.get());
    ASSERT_NE(func, nullptr);

    ASSERT_EQ(func->parameters.size(), 2);
    EXPECT_EQ(func->parameters[0]->value, "x");
    EXPECT_EQ(func->parameters[1]->value, "y");

    ASSERT_EQ(func->body->statements.size(), 1);
    auto bodyStmt = dynamic_cast<AST::ExpressionStatement *>(func->body->statements[0].get());
    ASSERT_NE(bodyStmt, nullptr);

    auto infix = dynamic_cast<AST::InfixExpression *>(bodyStmt->expression.get());
    ASSERT_NE(infix, nullptr);
    EXPECT_EQ(infix->op, "+");
}

TEST_F(ParserTest, TestFunctionParameterParsing)
{
    struct Test
    {
        std::string input;
        std::vector<std::string> expectedParams;
    };

    std::vector<Test> tests = {
        {"fn() {};", {}},
        {"fn(x) {};", {"x"}},
        {"fn(x, y, z) {};", {"x", "y", "z"}},
    };

    for (const auto &tt : tests)
    {
        auto [program, parser_owner, parser] = ParseInput(tt.input);
        ASSERT_NE(program.get(), nullptr);
        ASSERT_TRUE(parser->Errors().empty()) << "Parser errors: " << parser->Errors()[0];

        auto stmt = dynamic_cast<AST::ExpressionStatement *>(program->statements[0].get());
        auto func = dynamic_cast<AST::FunctionLiteral *>(stmt->expression.get());

        ASSERT_EQ(func->parameters.size(), tt.expectedParams.size());

        for (size_t i = 0; i < tt.expectedParams.size(); i++)
        {
            EXPECT_EQ(func->parameters[i]->value, tt.expectedParams[i]);
        }
    }
}

TEST_F(ParserTest, TestCallExpression)
{
    const std::string input = "add(1, 2 * 3, 4 + 5);";

    auto [program, parser_owner, parser] = ParseInput(input);
    ASSERT_NE(program.get(), nullptr);
    ASSERT_TRUE(parser->Errors().empty()) << "Parser errors: " << parser->Errors()[0];
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<AST::ExpressionStatement *>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);

    auto exp = dynamic_cast<AST::CallExpression *>(stmt->expression.get());
    ASSERT_NE(exp, nullptr);

    auto ident = dynamic_cast<AST::Identifier *>(exp->function.get());
    ASSERT_NE(ident, nullptr);
    EXPECT_EQ(ident->value, "add");

    ASSERT_EQ(exp->arguments.size(), 3);

    // First argument
    auto first = dynamic_cast<AST::IntegerLiteral *>(exp->arguments[0].get());
    ASSERT_NE(first, nullptr);
    EXPECT_EQ(first->value, 1);

    // Second argument (2 * 3)
    auto second = dynamic_cast<AST::InfixExpression *>(exp->arguments[1].get());
    ASSERT_NE(second, nullptr);
    EXPECT_EQ(second->op, "*");

    // Third argument (4 + 5)
    auto third = dynamic_cast<AST::InfixExpression *>(exp->arguments[2].get());
    ASSERT_NE(third, nullptr);
    EXPECT_EQ(third->op, "+");
}

TEST_F(ParserTest, TestCallExpressionParameter)
{
    struct Test
    {
        std::string input;
        std::string expectedIdent;
        std::vector<std::string> expectedArgs;
    };

    std::vector<Test> tests = {
        {"add();", "add", {}},
        {"add(1);", "add", {"1"}},
        {"add(1, 2 * 3, 4 + 5);", "add", {"1", "(2 * 3)", "(4 + 5)"}},
    };

    for (const auto &tt : tests)
    {
        auto [program, parser_owner, parser] = ParseInput(tt.input);
        ASSERT_NE(program.get(), nullptr);
        ASSERT_TRUE(parser->Errors().empty()) << "Parser errors: " << parser->Errors()[0];

        auto stmt = dynamic_cast<AST::ExpressionStatement *>(program->statements[0].get());
        auto exp = dynamic_cast<AST::CallExpression *>(stmt->expression.get());

        auto ident = dynamic_cast<AST::Identifier *>(exp->function.get());
        EXPECT_EQ(ident->value, tt.expectedIdent);

        ASSERT_EQ(exp->arguments.size(), tt.expectedArgs.size());

        for (size_t i = 0; i < tt.expectedArgs.size(); i++)
        {
            std::string argStr = exp->arguments[i]->String();
            EXPECT_EQ(argStr, tt.expectedArgs[i]);
        }
    }
}

TEST_F(ParserTest, TestReturnStatements)
{
    std::string input = R"(
        return 5;
        return 10;
        return 993322;
    )";

    Lexer::Lexer lexer(input);
    Parser::Parser parser(std::make_unique<Lexer::Lexer>(std::move(lexer)));

    auto program = parser.ParseProgram();
    CheckParserErrors(parser);

    ASSERT_NE(program, nullptr) << "ParseProgram() returned nullptr";
    CheckParserErrors(parser);
    ASSERT_EQ(program->statements.size(), 3);

    auto stmt1 = dynamic_cast<AST::ReturnStatement *>(program->statements[0].get());
    ASSERT_NE(stmt1, nullptr) << "Not a ReturnStatement";
    EXPECT_EQ(stmt1->TokenLiteral(), "return");

    auto value1 = dynamic_cast<AST::IntegerLiteral *>(stmt1->returnValue.get());
    ASSERT_NE(value1, nullptr);
    EXPECT_EQ(value1->value, 5);

    auto stmt2 = dynamic_cast<AST::ReturnStatement *>(program->statements[1].get());
    ASSERT_NE(stmt2, nullptr) << "Not a ReturnStatement";
    auto value2 = dynamic_cast<AST::IntegerLiteral *>(stmt2->returnValue.get());
    ASSERT_NE(value2, nullptr);
    EXPECT_EQ(value2->value, 10);

    auto stmt3 = dynamic_cast<AST::ReturnStatement *>(program->statements[2].get());
    ASSERT_NE(stmt3, nullptr) << "Not a ReturnStatement";
    auto value3 = dynamic_cast<AST::IntegerLiteral *>(stmt3->returnValue.get());
    ASSERT_NE(value3, nullptr);
    EXPECT_EQ(value3->value, 993322);
}

TEST_F(ParserTest, TestArrayLiterals)
{
    struct TestCase
    {
        std::string input;
        std::vector<std::string> expected;
    };

    std::vector<TestCase> tests = {
        {"[1, 2 * 2, 3 + 3]", {"1", "(2 * 2)", "(3 + 3)"}},
        {"[]", {}},
        {"[1]", {"1"}},
        {"[1, 2, 3]", {"1", "2", "3"}},
    };

    for (const auto &tt : tests)
    {
        auto [program, parser_owner, parser] = ParseInput(tt.input);
        ASSERT_NE(program.get(), nullptr);
        ASSERT_TRUE(parser->Errors().empty());
        ASSERT_EQ(program->statements.size(), 1);

        auto stmt = dynamic_cast<AST::ExpressionStatement *>(program->statements[0].get());
        ASSERT_NE(stmt, nullptr);

        auto array = dynamic_cast<AST::ArrayLiteral *>(stmt->expression.get());
        ASSERT_NE(array, nullptr);
        ASSERT_EQ(array->elements.size(), tt.expected.size());

        for (size_t i = 0; i < tt.expected.size(); i++)
        {
            ASSERT_NE(array->elements[i], nullptr);
            EXPECT_EQ(array->elements[i]->String(), tt.expected[i]);
        }
    }
}

TEST_F(ParserTest, TestIndexExpressions)
{
    std::string input = "myArray[1 + 1]";

    auto [program, parser_owner, parser] = ParseInput(input);
    ASSERT_NE(program.get(), nullptr);
    ASSERT_TRUE(parser->Errors().empty());
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<AST::ExpressionStatement *>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);

    auto indexExp = dynamic_cast<AST::IndexExpression *>(stmt->expression.get());
    ASSERT_NE(indexExp, nullptr);

    auto ident = dynamic_cast<AST::Identifier *>(indexExp->left.get());
    ASSERT_NE(ident, nullptr);
    EXPECT_EQ(ident->value, "myArray");

    auto index = dynamic_cast<AST::InfixExpression *>(indexExp->index.get());
    ASSERT_NE(index, nullptr);
    EXPECT_EQ(index->op, "+");

    auto left = dynamic_cast<AST::IntegerLiteral *>(index->left.get());
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->value, 1);

    auto right = dynamic_cast<AST::IntegerLiteral *>(index->right.get());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->value, 1);
}

// ヘルパー関数を追加
void testIntegerLiteral(AST::Expression *expr, int64_t value)
{
    auto intLit = dynamic_cast<AST::IntegerLiteral *>(expr);
    ASSERT_NE(intLit, nullptr) << "Expression is not IntegerLiteral";
    EXPECT_EQ(intLit->value, value);
}

void testIdentifier(AST::Expression *expr, const std::string &value)
{
    auto ident = dynamic_cast<AST::Identifier *>(expr);
    ASSERT_NE(ident, nullptr) << "Expression is not Identifier";
    EXPECT_EQ(ident->value, value);
}

void testInfixExpression(AST::Expression *expr, int64_t left_value,
                         const std::string &operator_literal, int64_t right_value)
{
    auto opExpr = dynamic_cast<AST::InfixExpression *>(expr);
    ASSERT_NE(opExpr, nullptr) << "Expression is not InfixExpression";

    EXPECT_EQ(opExpr->op, operator_literal);

    auto leftExpr = dynamic_cast<AST::IntegerLiteral *>(opExpr->left.get());
    ASSERT_NE(leftExpr, nullptr) << "Left expression is not IntegerLiteral";
    EXPECT_EQ(leftExpr->value, left_value);

    auto rightExpr = dynamic_cast<AST::IntegerLiteral *>(opExpr->right.get());
    ASSERT_NE(rightExpr, nullptr) << "Right expression is not IntegerLiteral";
    EXPECT_EQ(rightExpr->value, right_value);
}

TEST_F(ParserTest, TestWhileExpression)
{
    std::string input = R"(
        while (x < 5) {
            let x = x + 1;
        }
    )";

    auto [program, parser_owner, parser] = ParseInput(input);
    ASSERT_NE(program.get(), nullptr);
    
    // エラーがある場合は内容を出力
    if (!parser->Errors().empty()) {
        std::cout << "Parser errors:" << std::endl;
        for (const auto& err : parser->Errors()) {
            std::cout << "\t" << err << std::endl;
        }
    }
    ASSERT_TRUE(parser->Errors().empty());

    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<AST::ExpressionStatement*>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);

    auto whileExpr = dynamic_cast<AST::WhileExpression*>(stmt->expression.get());
    ASSERT_NE(whileExpr, nullptr);

    // 条件式のテスト
    auto condition = dynamic_cast<AST::InfixExpression*>(whileExpr->condition.get());
    ASSERT_NE(condition, nullptr);
    EXPECT_EQ(condition->op, "<");

    auto left = dynamic_cast<AST::Identifier*>(condition->left.get());
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->value, "x");

    auto right = dynamic_cast<AST::IntegerLiteral*>(condition->right.get());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->value, 5);

    // ブロック文のテスト
    ASSERT_NE(whileExpr->body, nullptr);
    ASSERT_EQ(whileExpr->body->statements.size(), 1);
}