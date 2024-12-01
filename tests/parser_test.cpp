#include <gtest/gtest.h>
#include "../parser/parser.hpp"
#include "../lexer/lexer.hpp"

TEST(ParserTest, TestLetStatements)
{
  struct TestCase
  {
    std::string input;
    std::string expectedIdentifier;
  };

  std::vector<TestCase> tests = {
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

    const auto &errors = parser.Errors();
    ASSERT_TRUE(errors.empty()) << "parser has " << errors.size() << " errors";

    ASSERT_EQ(program->statements.size(), 1)
        << "program.statements does not contain 1 statement. got="
        << program->statements.size();

    auto stmt = dynamic_cast<AST::LetStatement *>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr) << "program.statements[0] is not LetStatement";

    EXPECT_EQ(stmt->TokenLiteral(), "let")
        << "stmt.TokenLiteral not 'let'. got=" << stmt->TokenLiteral();

    EXPECT_EQ(stmt->name->value, tt.expectedIdentifier)
        << "letStmt.name.value not '" << tt.expectedIdentifier
        << "'. got=" << stmt->name->value;

    EXPECT_EQ(stmt->name->TokenLiteral(), tt.expectedIdentifier)
        << "letStmt.name.TokenLiteral() not '" << tt.expectedIdentifier
        << "'. got=" << stmt->name->TokenLiteral();
  }
}

TEST(ParserTest, TestReturnStatements)
{
  std::string input = R"(
        return 5;
        return 10;
        return 993322;
    )";

  Lexer::Lexer lexer(input);
  Parser::Parser parser(lexer);
  auto program = parser.ParseProgram();

  ASSERT_NE(program, nullptr) << "ParseProgram() returned nullptr";

  const auto &errors = parser.Errors();
  ASSERT_TRUE(errors.empty()) << "parser has " << errors.size() << " errors";

  ASSERT_EQ(program->statements.size(), 3)
      << "program.statements does not contain 3 statements. got="
      << program->statements.size();

  for (const auto &stmt : program->statements)
  {
    auto returnStmt = dynamic_cast<AST::ReturnStatement *>(stmt.get());
    ASSERT_NE(returnStmt, nullptr) << "stmt not ReturnStatement";
    EXPECT_EQ(returnStmt->TokenLiteral(), "return")
        << "returnStmt.TokenLiteral not 'return'. got="
        << returnStmt->TokenLiteral();
  }
}

TEST(ParserTest, TestLetStatementErrors)
{
  const std::string input = R"(
        let = 5;
        let x 5;
        let 838383;
    )";

  Lexer::Lexer l(input);
  Parser::Parser p(l);

  auto program = p.ParseProgram();
  auto errors = p.Errors();

  // エラーが3つあることを確認
  ASSERT_GE(errors.size(), 3);

  // エラーメッセージの内容をチェック
  bool hasIdentError = false;
  bool hasAssignError = false;
  bool hasExpressionError = false;

  for (const auto &error : errors)
  {
    if (error.find("IDENT") != std::string::npos)
    {
      hasIdentError = true;
    }
    if (error.find("ASSIGN") != std::string::npos)
    {
      hasAssignError = true;
    }
    if (error.find("expression") != std::string::npos)
    {
      hasExpressionError = true;
    }
  }

  EXPECT_TRUE(hasIdentError) << "Expected IDENT error";
  EXPECT_TRUE(hasAssignError) << "Expected ASSIGN error";
}

TEST(ParserTest, TestInvalidExpressionError)
{
  const std::string input = "!;";

  Lexer::Lexer l(input);
  Parser::Parser p(l);

  auto program = p.ParseProgram();
  auto errors = p.Errors();

  ASSERT_FALSE(errors.empty());
  bool hasError = false;
  for (const auto &error : errors)
  {
    if (error.find("parse") != std::string::npos ||
        error.find("expression") != std::string::npos)
    {
      hasError = true;
      break;
    }
  }
  EXPECT_TRUE(hasError) << "Expected parsing error message";
}

TEST(ParserTest, TestMissingSemicolonError)
{
  const std::string input = R"(
        let x = 5
        let y = 10;
    )";

  Lexer::Lexer l(input);
  Parser::Parser p(l);

  auto program = p.ParseProgram();
  auto errors = p.Errors();

  ASSERT_FALSE(errors.empty());
  bool hasError = false;
  for (const auto &error : errors)
  {
    if (error.find("semicolon") != std::string::npos ||
        error.find("expression") != std::string::npos)
    {
      hasError = true;
      break;
    }
  }
  EXPECT_TRUE(hasError) << "Expected semicolon error message";
}

TEST(ParserTest, TestReturnStatementError)
{
  const std::string input = "return";

  Lexer::Lexer l(input);
  Parser::Parser p(l);

  auto program = p.ParseProgram();
  auto errors = p.Errors();

  ASSERT_FALSE(errors.empty());
  bool hasError = false;
  for (const auto &error : errors)
  {
    if (error.find("Failed to parse return value") != std::string::npos ||
        error.find("expression") != std::string::npos)
    {
      hasError = true;
      break;
    }
  }
  EXPECT_TRUE(hasError) << "Expected return value error message";
}

TEST(ParserTest, TestOperatorPrecedenceParsing)
{
  struct Test
  {
    std::string input;
    std::string expected;
  };

  std::vector<Test> tests = {
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
    Lexer::Lexer l(tt.input);
    Parser::Parser p(l);
    auto program = p.ParseProgram();

    ASSERT_NE(program, nullptr);
    const auto &errors = p.Errors();
    ASSERT_TRUE(errors.empty()) << "Parser errors: " << errors[0];

    auto actual = program->String();
    // 空白とセミコロンを無視して比較
    actual.erase(std::remove(actual.begin(), actual.end(), ' '), actual.end());
    actual.erase(std::remove(actual.begin(), actual.end(), ';'), actual.end());
    std::string expected = tt.expected;
    expected.erase(std::remove(expected.begin(), expected.end(), ' '), expected.end());

    EXPECT_EQ(actual, expected)
        << "input=" << tt.input
        << " expected=" << tt.expected
        << " got=" << actual;
  }
}

TEST(ParserTest, TestIdentifierExpression)
{
  const std::string input = "foobar;";

  Lexer::Lexer l(input);
  Parser::Parser p(l);
  auto program = p.ParseProgram();

  ASSERT_NE(program, nullptr);
  ASSERT_TRUE(p.Errors().empty());
  ASSERT_EQ(program->statements.size(), 1);

  auto stmt = dynamic_cast<AST::ExpressionStatement *>(program->statements[0].get());
  ASSERT_NE(stmt, nullptr);

  auto ident = dynamic_cast<AST::Identifier *>(stmt->expression.get());
  ASSERT_NE(ident, nullptr);
  EXPECT_EQ(ident->value, "foobar");
  EXPECT_EQ(ident->TokenLiteral(), "foobar");
}

TEST(ParserTest, TestIntegerLiteralExpression)
{
  const std::string input = "5;";

  Lexer::Lexer l(input);
  Parser::Parser p(l);
  auto program = p.ParseProgram();

  ASSERT_NE(program, nullptr);
  ASSERT_TRUE(p.Errors().empty());
  ASSERT_EQ(program->statements.size(), 1);

  auto stmt = dynamic_cast<AST::ExpressionStatement *>(program->statements[0].get());
  ASSERT_NE(stmt, nullptr);

  auto literal = dynamic_cast<AST::IntegerLiteral *>(stmt->expression.get());
  ASSERT_NE(literal, nullptr);
  EXPECT_EQ(literal->value, 5);
  EXPECT_EQ(literal->TokenLiteral(), "5");
}

TEST(ParserTest, TestPrefixExpressions)
{
  struct PrefixTest
  {
    std::string input;
    std::string op;
    int64_t value;
  };

  std::vector<PrefixTest> tests = {
      {"!5;", "!", 5},
      {"-15;", "-", 15},
  };

  for (const auto &tt : tests)
  {
    Lexer::Lexer l(tt.input);
    Parser::Parser p(l);
    auto program = p.ParseProgram();

    ASSERT_NE(program, nullptr);
    ASSERT_TRUE(p.Errors().empty());
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

TEST(ParserTest, TestInfixExpressions)
{
  struct InfixTest
  {
    std::string input;
    int64_t leftValue;
    std::string op;
    int64_t rightValue;
  };

  std::vector<InfixTest> tests = {
      {"5 + 5;", 5, "+", 5},
      {"5 - 5;", 5, "-", 5},
      {"5 * 5;", 5, "*", 5},
      {"5 / 5;", 5, "/", 5},
      {"5 > 5;", 5, ">", 5},
      {"5 < 5;", 5, "<", 5},
      {"5 == 5;", 5, "==", 5},
      {"5 != 5;", 5, "!=", 5},
  };

  for (const auto &tt : tests)
  {
    Lexer::Lexer l(tt.input);
    Parser::Parser p(l);
    auto program = p.ParseProgram();

    ASSERT_NE(program, nullptr);
    ASSERT_TRUE(p.Errors().empty()) << "Parser errors: " << p.Errors()[0];
    ASSERT_EQ(program->statements.size(), 1);

    auto stmt = dynamic_cast<AST::ExpressionStatement *>(program->statements[0].get());
    ASSERT_NE(stmt, nullptr);

    auto exp = dynamic_cast<AST::InfixExpression *>(stmt->expression.get());
    ASSERT_NE(exp, nullptr);

    auto left = dynamic_cast<AST::IntegerLiteral *>(exp->left.get());
    ASSERT_NE(left, nullptr);
    EXPECT_EQ(left->value, tt.leftValue);

    EXPECT_EQ(exp->op, tt.op);

    auto right = dynamic_cast<AST::IntegerLiteral *>(exp->right.get());
    ASSERT_NE(right, nullptr);
    EXPECT_EQ(right->value, tt.rightValue);
  }
}

TEST(ParserTest, TestMultipleErrors)
{
  const std::string input = R"(
        let x 5;
        return;
        let = 10;
        let 838383;
    )";

  Lexer::Lexer l(input);
  Parser::Parser p(l);

  auto program = p.ParseProgram();
  auto errors = p.Errors();

  // 複数のエラーが検出されることを確認
  ASSERT_GT(errors.size(), 2);

  // エラーメッセージの内容をチェック
  bool hasAssignError = false;
  bool hasIdentError = false;

  for (const auto &error : errors)
  {
    if (error.find("ASSIGN") != std::string::npos)
    {
      hasAssignError = true;
    }
    if (error.find("IDENT") != std::string::npos)
    {
      hasIdentError = true;
    }
  }

  EXPECT_TRUE(hasAssignError);
  EXPECT_TRUE(hasIdentError);
}