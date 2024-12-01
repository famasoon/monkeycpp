cmake_minimum_required(VERSION 3.10)
project(monkey)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GoogleTestのダウンロードと設定
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
)
FetchContent_MakeAvailable(googletest)

# インクルードディレクトリの設定
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${gtest_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/ast
    ${CMAKE_SOURCE_DIR}/evaluator
    ${CMAKE_SOURCE_DIR}/object
    ${CMAKE_SOURCE_DIR}/parser
    ${CMAKE_SOURCE_DIR}/lexer
    ${CMAKE_SOURCE_DIR}/token
)

# objectライブラリの追加
add_library(object
    object/object.cpp
)

# メインのプログラムのソースファイル
add_library(monkey_lib
    token/token.cpp
    lexer/lexer.cpp
    ast/ast.cpp
    parser/parser.cpp
    repl/repl.cpp
)

# メインの実行ファイル
add_executable(monkey main.cpp)
target_link_libraries(monkey monkey_lib object)

# evaluatorライブラリの追加
add_library(evaluator
    evaluator/evaluator.cpp
)
target_link_libraries(evaluator
    object
    monkey_lib
)

# テストの設定
enable_testing()

# lexer_parserのテスト
add_executable(monkey_test
    tests/lexer_test.cpp
    tests/parser_test.cpp
)
target_link_libraries(monkey_test
    PRIVATE
    GTest::gtest_main
    GTest::gtest
    monkey_lib
    object
)

# evaluatorのテスト
add_executable(evaluator_test
    tests/evaluator_test.cpp
)
target_link_libraries(evaluator_test
    PRIVATE
    gtest
    gtest_main
    evaluator
    monkey_lib
    object
)

include(GoogleTest)
gtest_discover_tests(monkey_test)
gtest_discover_tests(evaluator_test) 