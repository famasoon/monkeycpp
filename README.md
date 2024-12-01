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

```bash
make test
```

## 参考

- [Monkey Programming Language](https://monkeylang.org/)
