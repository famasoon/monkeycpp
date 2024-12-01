cmake_minimum_required(VERSION 3.10)
project(monkey_interpreter)

# コンパイラ設定
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# コンパイルオプション
add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Werror
)

# ソースファイルの設定
set(SOURCES
    token/token.cpp
    lexer/lexer.cpp
    parser/parser.cpp
    ast/ast.cpp
    repl/repl.cpp
)

# メインの実行ファイル
add_executable(monkey 
    main.cpp 
    ${SOURCES}
)

# テストの設定
enable_testing()

# Google Testのダウンロードと設定
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
)
# Google Testのビルド設定
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(BUILD_GTEST ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# インクルードパスの設定を追加
include_directories(${CMAKE_SOURCE_DIR})

# テストの実行ファイル
add_executable(monkey_test
    tests/lexer_test.cpp
    tests/parser_test.cpp
    ${SOURCES}
)

# テストのリンク設定
target_link_libraries(monkey_test PRIVATE 
    gtest_main
)

# テストのコンパイルフラグ設定
target_compile_options(monkey_test PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

# テストの追加（WORKING_DIRECTORY を追加）
add_test(
    NAME lexer_test 
    COMMAND monkey_test --gtest_filter=LexerTest.*
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
add_test(
    NAME parser_test 
    COMMAND monkey_test --gtest_filter=ParserTest.*
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# テストのプロパティ設定
set_tests_properties(lexer_test parser_test
    PROPERTIES
    ENVIRONMENT "GTEST_COLOR=1"
    TIMEOUT 10
)

# インストール設定
install(TARGETS monkey
    RUNTIME DESTINATION bin
) 