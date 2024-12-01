# Monkey Programming Language Interpreter

C++で実装されたMonkeyプログラミング言語のインタープリタです。

## 必要条件

- CMake (バージョン3.10以上)
- C++17対応のコンパイラ
- Git

## ビルド手順

```bash
git clone https://github.com/kazuki-kawabata/monkey-interpreter.git
cd monkey-interpreter
mkdir build
cd build
cmake ..
make
```

## 実行方法

```bash
./monkey
```

## テスト

### テストのビルドと実行

```bash
# ビルドディレクトリで以下を実行
cmake ..
make
```

### すべてのテストを実行

```bash
# 基本的な実行
ctest

# 詳細な出力を表示
ctest --output-on-failure

# 並列実行（4つのジョブを同時実行）
ctest -j4
```

### 特定のテストを実行

```bash
# レキサーのテストのみ実行
./monkey_test --gtest_filter=LexerTest.*

# パーサーのテストのみ実行
./monkey_test --gtest_filter=ParserTest.*

# テスト結果の詳細を表示
./monkey_test --gtest_color=yes --gtest_print_time=1
```

### テストの内容

- `tests/lexer_test.cpp`: レキサーのテスト
  - トークン化の正確性
  - 各種トークンの認識

- `tests/parser_test.cpp`: パーサーのテスト
  - let文のパース
  - return文のパース

## プロジェクト構造

```
.
├── CMakeLists.txt
├── main.cpp
├── token/
│   ├── token.hpp
│   └── token.cpp
├── lexer/
│   ├── lexer.hpp
│   └── lexer.cpp
├── parser/
│   ├── parser.hpp
│   └── parser.cpp
├── ast/
│   ├── ast.hpp
│   └── ast.cpp
├── repl/
│   ├── repl.hpp
│   └── repl.cpp
└── tests/
    ├── lexer_test.cpp
    └── parser_test.cpp
```

## 参考

- [Monkey Programming Language](https://monkeylang.org/)
- [Google Test](https://google.github.io/googletest/)
