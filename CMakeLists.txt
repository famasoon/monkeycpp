cmake_minimum_required(VERSION 3.10)
project(monkey)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# GoogleTestのダウンロードと設定
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
FetchContent_MakeAvailable(googletest)

# メインのプログラムのソースファイル
set(SOURCES
    token/token.hpp
    token/token.cpp
    lexer/lexer.hpp
    lexer/lexer.cpp
    ast/ast.hpp
    ast/ast.cpp
    parser/parser.hpp
    parser/parser.cpp
    repl/repl.hpp
    repl/repl.cpp
)

# メインの実行ファイル
add_executable(monkey ${SOURCES} main.cpp)

# テストの設定
enable_testing()

# テストのソースファイル
set(TEST_SOURCES
    tests/lexer_test.cpp
    tests/parser_test.cpp
)

# テスト実行ファイル
add_executable(monkey_test ${SOURCES} ${TEST_SOURCES})
target_link_libraries(monkey_test GTest::gtest_main GTest::gtest)

include(GoogleTest)
gtest_discover_tests(monkey_test) 