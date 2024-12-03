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
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# オブジェクトライブラリ
add_library(object
    object/object.cpp
)

# 評価器ライブラリ
add_library(evaluator
    evaluator/evaluator.cpp
)
target_link_libraries(evaluator
    object
)

# メインライブラリ
add_library(monkey_lib
    token/token.cpp
    lexer/lexer.cpp
    ast/ast.cpp
    parser/parser.cpp
    repl/repl.cpp
)
target_link_libraries(monkey_lib
    object
    evaluator
)

# 実行可能ファイル
add_executable(monkey
    main.cpp
)
target_link_libraries(monkey
    monkey_lib
    evaluator
)

# テスト関連
enable_testing()

# レキサーテスト
add_executable(lexer_test
    tests/lexer_test.cpp
)
target_link_libraries(lexer_test
    monkey_lib
    GTest::gtest
    GTest::gtest_main
)

# パーサーテスト
add_executable(parser_test
    tests/parser_test.cpp
)
target_link_libraries(parser_test
    monkey_lib
    GTest::gtest
    GTest::gtest_main
)

# 評価器テスト
add_executable(evaluator_test
    tests/evaluator_test.cpp
)
target_link_libraries(evaluator_test
    monkey_lib
    evaluator
    GTest::gtest
    GTest::gtest_main
)

add_test(NAME lexer_test COMMAND lexer_test)
add_test(NAME parser_test COMMAND parser_test)
add_test(NAME evaluator_test COMMAND evaluator_test)

# 既存の設定に追加
target_compile_features(monkey PRIVATE cxx_std_17)

# LLVMの設定
if(APPLE)
    execute_process(
        COMMAND brew --prefix llvm
        OUTPUT_VARIABLE LLVM_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(LLVM_DIR "${LLVM_PREFIX}/lib/cmake/llvm")
else()
    # Ubuntu用の設定
    set(LLVM_DIR "/usr/lib/llvm-14/lib/cmake/llvm")  # バージョンは環境に応じて変更
endif()

find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# JITライブラリの追加
add_library(jit
    jit/jit.cpp
)
target_include_directories(jit PRIVATE ${LLVM_INCLUDE_DIRS})
target_link_libraries(jit
    monkey_lib
    LLVM
)

# メインターゲットにJITを追加
target_link_libraries(monkey
    monkey_lib
    evaluator
    jit
) 