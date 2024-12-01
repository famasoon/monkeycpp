#include <gtest/gtest.h>
#include "../parser/parser.hpp"
#include "../lexer/lexer.hpp"
#include <algorithm>

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
    auto lexer = Lexer::Lexer(input);
    auto parser = std::make_unique<Parser::Parser>(std::move(lexer));
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
    return std::any_of(errors.begin(), errors.end(),
                       [&](const std::string &error)
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
}

class ParserTest : public ::testing::Test
{
protected:
  void SetUp() override {}
  void TearDown() override {}
};

// Let文のテスト
TEST_F(ParserTest, TestLetStatements)
{
  std::vector<LetStatementTestCase> tests = {
      {"let x = 5;", "x"},
      {"let y = 10;", "y"},
      {"let foobar = 838383;", "foobar"},
  };

  for (const auto &tt : tests)
  {
    Lexer::Lexer lexer(tt.input);
    Parser::Parser parser(lexer);
    auto program = parser.ParseProgram();

    ASSERT_NE(program, nullptr) << "ParseProgram() returned nullptr";
    CheckParserErrors(parser);
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<AST::LetStatement *>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr) << "Not a LetStatement";
    EXPECT_EQ(stmt->TokenLiteral(), "let");
    EXPECT_EQ(stmt->name->value, tt.expectedIdentifier);
    EXPECT_EQ(stmt->name->TokenLiteral(), tt.expectedIdentifier);
  }
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
                                [](char c)
                                { return c == ' ' || c == ';'; }),
                 actual.end());

    std::string expected = tt.expected;
    expected.erase(std::remove(expected.begin(), expected.end(), ' '), expected.end());

    EXPECT_EQ(actual, expected)
        << "Input: " << tt.input
        << "\nExpected: " << tt.expected
        << "\nGot: " << actual;
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