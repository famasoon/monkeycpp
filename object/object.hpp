#pragma once
#include <string>
#include <memory>
#include <variant>

namespace monkey {

// オブジェクトの型を表現する列挙型
enum class ObjectType {
    INTEGER,
    BOOLEAN,
    STRING,
    NULL_OBJ,
    ERROR
};

// 前方宣言
class Object;
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

} // namespace monkey 