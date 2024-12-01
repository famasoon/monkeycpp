#include <gtest/gtest.h>
#include "../evaluator/evaluator.hpp"
#include "../lexer/lexer.hpp"
#include "../parser/parser.hpp"
#include "../object/object.hpp"

using namespace monkey;

// ヘルパー関数
ObjectPtr testEval(const std::string& input) {
    auto lexer = std::make_unique<Lexer::Lexer>(input);
    auto parser = std::make_unique<Parser::Parser>(std::move(lexer));
    auto program = parser->ParseProgram();
    
    if (!program || program->statements.empty()) {
        return std::make_shared<Error>("Failed to parse program");
    }
    
    Evaluator evaluator;
    return evaluator.eval(program.get());
}

// 整数の評価をテストするヘルパー関数
void testIntegerObject(const ObjectPtr& obj, int64_t expected) {
    auto integer = std::dynamic_pointer_cast<Integer>(obj);
    ASSERT_NE(integer, nullptr);
    EXPECT_EQ(integer->value(), expected);
}

// 真偽値の評価をテストするヘルパー関数
void testBooleanObject(const ObjectPtr& obj, bool expected) {
    auto boolean = std::dynamic_pointer_cast<Boolean>(obj);
    ASSERT_NE(boolean, nullptr);
    EXPECT_EQ(boolean->value(), expected);
}

// 文字列の評価をテストするヘルパー関数
void testStringObject(const ObjectPtr& obj, const std::string& expected) {
    auto str = std::dynamic_pointer_cast<String>(obj);
    ASSERT_NE(str, nullptr);
    EXPECT_EQ(str->value(), expected);
}

// 整数リテラルのテスト
TEST(EvaluatorTest, TestEvalIntegerExpression) {
    struct Test {
        std::string input;
        int64_t expected;
    };

    std::vector<Test> tests = {
        {"5", 5},
        {"10", 10},
        {"-5", -5},
        {"-10", -10},
        {"5 + 5", 10},
        {"5 - 5", 0},
        {"5 * 5", 25},
        {"5 / 5", 1},
        {"2 * (5 + 5)", 20},
        {"3 * 3 * 3", 27},
        {"(5 + 10 * 2 + 15 / 3)", 30},
    };

    for (const auto& tt : tests) {
        auto evaluated = testEval(tt.input);
        testIntegerObject(evaluated, tt.expected);
    }
}

// 真偽値のテスト
TEST(EvaluatorTest, TestEvalBooleanExpression) {
    struct Test {
        std::string input;
        bool expected;
    };

    std::vector<Test> tests = {
        {"true", true},
        {"false", false},
        {"1 < 2", true},
        {"1 > 2", false},
        {"1 == 1", true},
        {"1 != 1", false},
        {"true == true", true},
        {"false == false", true},
        {"true != false", true},
        {"(1 < 2) == true", true},
    };

    for (const auto& tt : tests) {
        auto evaluated = testEval(tt.input);
        testBooleanObject(evaluated, tt.expected);
    }
}

// 文字列のテスト
TEST(EvaluatorTest, TestEvalStringExpression) {
    struct Test {
        std::string input;
        std::string expected;
    };

    std::vector<Test> tests = {
        {"\"Hello World!\"", "Hello World!"},
        {"\"Hello\" + \" \" + \"World!\"", "Hello World!"},
    };

    for (const auto& tt : tests) {
        auto evaluated = testEval(tt.input);
        testStringObject(evaluated, tt.expected);
    }
}

// 前置演算子のテスト
TEST(EvaluatorTest, TestBangOperator) {
    struct Test {
        std::string input;
        bool expected;
    };

    std::vector<Test> tests = {
        {"!true", false},
        {"!false", true},
        {"!!true", true},
        {"!!false", false},
        {"!5", false},
        {"!!5", true},
    };

    for (const auto& tt : tests) {
        auto evaluated = testEval(tt.input);
        testBooleanObject(evaluated, tt.expected);
    }
}

// エラー処理のテスト
TEST(EvaluatorTest, TestErrorHandling) {
    struct Test {
        std::string input;
        std::string expectedMessage;
    };

    std::vector<Test> tests = {
        {"5 + true", "type mismatch: INTEGER + BOOLEAN"},
        {"5 + true; 5;", "type mismatch: INTEGER + BOOLEAN"},
        {"-true", "invalid operator: -BOOLEAN"},
        {"true + false", "unknown operator: BOOLEAN + BOOLEAN"},
        {"5; true + false; 5", "unknown operator: BOOLEAN + BOOLEAN"},
        {"if (10 > 1) { true + false; }", "unknown operator: BOOLEAN + BOOLEAN"},
        {"\"Hello\" - \"World\"", "unknown operator: STRING - STRING"},
    };

    for (const auto& tt : tests) {
        auto evaluated = testEval(tt.input);
        auto errorObj = std::dynamic_pointer_cast<Error>(evaluated);
        ASSERT_NE(errorObj, nullptr) << "Expected error object, got: " 
                                    << (evaluated ? evaluated->inspect() : "nullptr");
        EXPECT_EQ(errorObj->message(), tt.expectedMessage);
    }
} 