#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <functional>  // std::function用
#include <variant>     // std::variant用
#include "../ast/ast.hpp"

namespace monkey
{

  // 前方宣言
  class Object;
  using ObjectPtr = std::shared_ptr<Object>;
  using EnvPtr = std::shared_ptr<class Environment>;
  using WeakEnvPtr = std::weak_ptr<class Environment>;

  // オブジェクトの種類を表す列挙型
  enum class ObjectType
  {
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

  // 基底クラス
  class Object
  {
  public:
    virtual ~Object() = default;
    virtual ObjectType type() const = 0;
    virtual std::string inspect() const = 0;
  };

  // HashKeyインターフェースを追加
  class HashKey {
  public:
    virtual ~HashKey() = default;
    virtual size_t hash() const = 0;
    virtual bool operator==(const HashKey& other) const = 0;
  };

  // 整数オブジェクト
  class Integer : public Object, public HashKey
  {
  private:
    int64_t value_;

  public:
    explicit Integer(int64_t value);
    ObjectType type() const override;
    std::string inspect() const override;
    int64_t value() const;
    size_t hash() const override {
        return std::hash<int64_t>{}(value_);
    }
    bool operator==(const HashKey& other) const override {
        if (auto* intKey = dynamic_cast<const Integer*>(&other)) {
            return value_ == intKey->value_;
        }
        return false;
    }
  };

  // 真偽値オブジェクト
  class Boolean : public Object, public HashKey
  {
  private:
    bool value_;

  public:
    explicit Boolean(bool value);
    ObjectType type() const override;
    std::string inspect() const override;
    bool value() const;
    size_t hash() const override {
        return std::hash<bool>{}(value_);
    }
    bool operator==(const HashKey& other) const override {
        if (auto* boolKey = dynamic_cast<const Boolean*>(&other)) {
            return value_ == boolKey->value_;
        }
        return false;
    }
  };

  // 文字列オブジェクト
  class String : public Object, public HashKey
  {
  private:
    std::string value_;

  public:
    explicit String(std::string value);
    ObjectType type() const override;
    std::string inspect() const override;
    const std::string &getValue() const;
    size_t hash() const override {
        return std::hash<std::string>{}(value_);
    }
    bool operator==(const HashKey& other) const override {
        if (auto* strKey = dynamic_cast<const String*>(&other)) {
            return value_ == strKey->getValue();
        }
        return false;
    }
  };

  // Nullオブジェクト
  class Null : public Object
  {
  public:
    ObjectType type() const override;
    std::string inspect() const override;
  };

  // エラーオブジェクト
  class Error : public Object
  {
  private:
    std::string message_;

  public:
    explicit Error(const std::string& message);
    ObjectType type() const override;
    std::string inspect() const override;
    const std::string& message() const;
  };

  // 戻り値オブジェクト
  class ReturnValue : public Object
  {
  public:
    ObjectPtr value;
    explicit ReturnValue(ObjectPtr v);
    ObjectType type() const override;
    std::string inspect() const override;
  };

  // 関数オブジェクト
  class Function : public Object
  {
  public:
    std::vector<std::string> parameters;
    const AST::BlockStatement* body;
    EnvPtr env;

    Function(std::vector<std::string> params, const AST::BlockStatement* b, EnvPtr e);
    ObjectType type() const override;
    std::string inspect() const override;
  };

  // ビルトイン関数の型定義
  using BuiltinFunction = std::function<ObjectPtr(const std::vector<ObjectPtr>&)>;

  // ビルトイン関数オブジェクト
  class Builtin : public Object
  {
  public:
    BuiltinFunction fn;
    explicit Builtin(BuiltinFunction function);
    ObjectType type() const override;
    std::string inspect() const override;
  };

  // 配列オブジェクト
  class Array : public Object
  {
  public:
    std::vector<ObjectPtr> elements;
    explicit Array(std::vector<ObjectPtr> elems);
    ObjectType type() const override;
    std::string inspect() const override;
  };

  // ハッシュペア
  class HashPair
  {
  public:
    ObjectPtr key;
    ObjectPtr value;
    HashPair() = default;
    HashPair(ObjectPtr k, ObjectPtr v) : key(k), value(v) {}
  };

  // ハッシュオブジェクト
  class Hash : public Object
  {
  public:
    std::unordered_map<size_t, HashPair> pairs;
    ObjectType type() const override;
    std::string inspect() const override;
  };

  // 環境クラス
  class Environment : public std::enable_shared_from_this<Environment>
  {
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