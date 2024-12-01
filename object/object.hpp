#pragma once
#include <string>
#include <memory>
#include <variant>
#include <vector>
#include <functional>      // std::function用
#include <unordered_map>  // std::unordered_map用
#include "../ast/ast.hpp"

namespace monkey {

// オブジェクトの型を表現する列挙型
enum class ObjectType {
    INTEGER,
    BOOLEAN,
    STRING,
    NULL_OBJ,
    ERROR,
    RETURN_VALUE,
    FUNCTION,
    BUILTIN,
    ARRAY,
    HASH
};

// 前方宣言
class Object;
class Environment;
using ObjectPtr = std::shared_ptr<Object>;

// 基本となるObjectクラス
class Object {
public:
    virtual ~Object() = default;
    virtual ObjectType type() const = 0;
    virtual std::string inspect() const = 0;
};

// 整数オブジェクト
class Integer : public Object {
public:
    explicit Integer(int64_t value);
    ObjectType type() const override;
    std::string inspect() const override;
    int64_t value() const;

private:
    int64_t value_;
};

// 真偽値オブジェクト
class Boolean : public Object {
public:
    explicit Boolean(bool value);
    ObjectType type() const override;
    std::string inspect() const override;
    bool value() const;

private:
    bool value_;
};

// 文字列オブジェクト
class String : public Object {
public:
    explicit String(const std::string& value);
    ObjectType type() const override;
    std::string inspect() const override;
    const std::string& value() const;

private:
    std::string value_;
};

// Nullオブジェクト
class Null : public Object {
public:
    ObjectType type() const override;
    std::string inspect() const override;
};

// エラーオブジェクト
class Error : public Object {
public:
    explicit Error(const std::string& message);
    ObjectType type() const override;
    std::string inspect() const override;
    const std::string& message() const;

private:
    std::string message_;
};

class ReturnValue : public Object {
public:
    ObjectPtr value;

    explicit ReturnValue(ObjectPtr v) : value(std::move(v)) {}

    ObjectType type() const override { return ObjectType::RETURN_VALUE; }
    std::string inspect() const override { return value ? value->inspect() : "null"; }
};

class Function : public Object {
public:
    std::vector<std::string> parameters;
    const AST::BlockStatement* body;
    std::shared_ptr<Environment> env;

    Function(std::vector<std::string> params, const AST::BlockStatement* b, std::shared_ptr<Environment> e)
        : parameters(std::move(params)), body(b), env(std::move(e)) {}

    ObjectType type() const override { return ObjectType::FUNCTION; }
    std::string inspect() const override;
};

using BuiltinFunction = std::function<ObjectPtr(const std::vector<ObjectPtr>&)>;

class Builtin : public Object {
public:
    BuiltinFunction fn;

    explicit Builtin(BuiltinFunction function) : fn(std::move(function)) {}

    ObjectType type() const override { return ObjectType::BUILTIN; }
    std::string inspect() const override { return "builtin function"; }
};

class Array : public Object {
public:
    std::vector<ObjectPtr> elements;

    explicit Array(std::vector<ObjectPtr> elems) : elements(std::move(elems)) {}

    ObjectType type() const override { return ObjectType::ARRAY; }
    std::string inspect() const override;
};

using HashKey = std::variant<int64_t, bool, std::string>;

class HashPair {
public:
    ObjectPtr key;
    ObjectPtr value;

    HashPair(ObjectPtr k, ObjectPtr v) : key(std::move(k)), value(std::move(v)) {}
};

class Hash : public Object {
public:
    std::unordered_map<HashKey, HashPair> pairs;

    ObjectType type() const override { return ObjectType::HASH; }
    std::string inspect() const override;
};

// ハッシュキーを生成するためのインターフェース
class Hashable {
public:
    virtual ~Hashable() = default;
    virtual HashKey hashKey() const = 0;
};

} // namespace monkey 