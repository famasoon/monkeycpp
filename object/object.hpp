#pragma once
#include <string>
#include <memory>
#include <variant>
#include <vector>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include "../ast/ast.hpp"

namespace monkey {

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

class Object;
class Environment;
using ObjectPtr = std::shared_ptr<Object>;
using EnvPtr = std::shared_ptr<Environment>;
using WeakEnvPtr = std::weak_ptr<Environment>;

class Object {
public:
    virtual ~Object() = default;
    virtual ObjectType type() const = 0;
    virtual std::string inspect() const = 0;
};

class Integer : public Object {
private:
    int64_t value_;
public:
    explicit Integer(int64_t value);
    ObjectType type() const override;
    std::string inspect() const override;
    int64_t value() const;
};

class Boolean : public Object {
private:
    bool value_;
public:
    explicit Boolean(bool value);
    ObjectType type() const override;
    std::string inspect() const override;
    bool value() const;
};

class String : public Object {
private:
    std::string value_;
public:
    explicit String(std::string value);
    ObjectType type() const override;
    std::string inspect() const override;
    const std::string& getValue() const;
};

class Null : public Object {
public:
    ObjectType type() const override;
    std::string inspect() const override;
};

class Error : public Object {
private:
    std::string message_;
public:
    explicit Error(const std::string& message);
    ObjectType type() const override;
    std::string inspect() const override;
    const std::string& message() const;
};

class ReturnValue : public Object {
public:
    ObjectPtr value;
    explicit ReturnValue(ObjectPtr v);
    ObjectType type() const override;
    std::string inspect() const override;
};

class Function : public Object {
public:
    std::vector<std::string> parameters;
    const AST::BlockStatement* body;
    EnvPtr env;

    Function(std::vector<std::string> params, const AST::BlockStatement* b, EnvPtr e);
    ObjectType type() const override;
    std::string inspect() const override;
};

using BuiltinFunction = std::function<ObjectPtr(const std::vector<ObjectPtr>&)>;

class Builtin : public Object {
public:
    BuiltinFunction fn;
    explicit Builtin(BuiltinFunction function);
    ObjectType type() const override;
    std::string inspect() const override;
};

class Array : public Object {
public:
    std::vector<ObjectPtr> elements;
    explicit Array(std::vector<ObjectPtr> elems);
    ObjectType type() const override;
    std::string inspect() const override;
};

using HashKey = std::variant<int64_t, bool, std::string>;

class HashPair {
public:
    ObjectPtr key;
    ObjectPtr value;
    HashPair(ObjectPtr k, ObjectPtr v);
};

class Hash : public Object {
public:
    std::unordered_map<HashKey, HashPair> pairs;
    ObjectType type() const override;
    std::string inspect() const override;
};

class Environment : public std::enable_shared_from_this<Environment> {
private:
    std::unordered_map<std::string, ObjectPtr> store;
    WeakEnvPtr outer;

public:
    static EnvPtr NewEnvironment();
    static EnvPtr NewEnclosedEnvironment(EnvPtr outer);
    ObjectPtr Get(const std::string& name);
    ObjectPtr Set(const std::string& name, ObjectPtr val);
    void MarkAndSweep();

private:
    void Mark(std::unordered_set<Object*>& marked);
    void MarkObject(Object* obj, std::unordered_set<Object*>& marked);
    void Sweep(const std::unordered_set<Object*>& marked);
};

} // namespace monkey