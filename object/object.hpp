#pragma once
#include <string>
#include <memory>
#include <variant>
#include <vector>
#include <functional>    // std::function用
#include <unordered_map> // std::unordered_map用
#include <unordered_set>
#include "../ast/ast.hpp"

namespace monkey
{

  // オブジェクトの型を表現する列挙型
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

  // 前方宣言
  class Object;
  class Environment;
  using ObjectPtr = std::shared_ptr<Object>;
  using EnvPtr = std::shared_ptr<Environment>;
  using WeakEnvPtr = std::weak_ptr<Environment>;

  // 基本となるObjectクラス
  class Object
  {
  public:
    virtual ~Object() = default;
    virtual ObjectType type() const = 0;
    virtual std::string inspect() const = 0;
  };

  // 整数オブジェクト
  class Integer : public Object
  {
  public:
    explicit Integer(int64_t value);
    ObjectType type() const override;
    std::string inspect() const override;
    int64_t value() const;

  private:
    int64_t value_;
  };

  // 真偽値オブジェクト
  class Boolean : public Object
  {
  public:
    explicit Boolean(bool value);
    ObjectType type() const override;
    std::string inspect() const override;
    bool value() const;

  private:
    bool value_;
  };

  // 文字列オブジェクト
  class String : public Object
  {
  public:
    explicit String(const std::string &value);
    ObjectType type() const override;
    std::string inspect() const override;
    const std::string &value() const;

  private:
    std::string value_;
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
  public:
    explicit Error(const std::string &message);
    ObjectType type() const override;
    std::string inspect() const override;
    const std::string &message() const;

  private:
    std::string message_;
  };

  class ReturnValue : public Object
  {
  public:
    ObjectPtr value;

    explicit ReturnValue(ObjectPtr v) : value(std::move(v)) {}

    ObjectType type() const override { return ObjectType::RETURN_VALUE; }
    std::string inspect() const override { return value ? value->inspect() : "null"; }
  };

  class Function : public Object
  {
  public:
    std::vector<std::string> parameters;
    const AST::BlockStatement *body;
    EnvPtr env;

    Function(std::vector<std::string> params, const AST::BlockStatement *b, EnvPtr e)
        : parameters(std::move(params)), body(b), env(std::move(e)) {}

    ObjectType type() const override { return ObjectType::FUNCTION; }
    std::string inspect() const override;
  };

  using BuiltinFunction = std::function<ObjectPtr(const std::vector<ObjectPtr> &)>;

  class Builtin : public Object
  {
  public:
    BuiltinFunction fn;

    explicit Builtin(BuiltinFunction function) : fn(std::move(function)) {}

    ObjectType type() const override { return ObjectType::BUILTIN; }
    std::string inspect() const override { return "builtin function"; }
  };

  class Array : public Object
  {
  public:
    std::vector<ObjectPtr> elements;

    explicit Array(std::vector<ObjectPtr> elems) : elements(std::move(elems)) {}

    ObjectType type() const override { return ObjectType::ARRAY; }
    std::string inspect() const override;
  };

  using HashKey = std::variant<int64_t, bool, std::string>;

  class HashPair
  {
  public:
    ObjectPtr key;
    ObjectPtr value;

    HashPair(ObjectPtr k, ObjectPtr v) : key(std::move(k)), value(std::move(v)) {}
  };

  class Hash : public Object
  {
  public:
    std::unordered_map<HashKey, HashPair> pairs;

    ObjectType type() const override { return ObjectType::HASH; }
    std::string inspect() const override;
  };

  // ハッシュキーを生成するためのインターフェース
  class Hashable
  {
  public:
    virtual ~Hashable() = default;
    virtual HashKey hashKey() const = 0;
  };

  // 環境クラス
  class Environment : public std::enable_shared_from_this<Environment>
  {
  private:
    std::unordered_map<std::string, ObjectPtr> store;
    WeakEnvPtr outer;

  public:
    static EnvPtr NewEnvironment()
    {
      return std::make_shared<Environment>();
    }

    static EnvPtr NewEnclosedEnvironment(EnvPtr outer)
    {
      auto env = NewEnvironment();
      env->outer = outer;
      return env;
    }

    ObjectPtr Get(const std::string &name)
    {
      auto it = store.find(name);
      if (it != store.end())
      {
        return it->second;
      }

      if (auto outerEnv = outer.lock())
      {
        return outerEnv->Get(name);
      }

      return nullptr;
    }

    ObjectPtr Set(const std::string &name, ObjectPtr val)
    {
      store[name] = val;
      return val;
    }

    // ガベージコレクション用のメソッド
    void MarkAndSweep()
    {
      std::unordered_set<Object *> marked;
      Mark(marked);
      Sweep(marked);
    }

  private:
    void Mark(std::unordered_set<Object *> &marked)
    {
      // 環境内のすべてのオブジェクトをマーク
      for (const auto &pair : store)
      {
        if (pair.second)
        {
          MarkObject(pair.second.get(), marked);
        }
      }

      // 外部環境も再帰的にマーク
      if (auto outerEnv = outer.lock())
      {
        outerEnv->Mark(marked);
      }
    }

    void MarkObject(Object *obj, std::unordered_set<Object *> &marked)
    {
      if (!obj || marked.find(obj) != marked.end())
      {
        return;
      }

      marked.insert(obj);

      // 特定の型に応じて子オブジェクトをマーク
      if (auto function = dynamic_cast<Function *>(obj))
      {
        if (function->env)
        {
          function->env->Mark(marked);
        }
      }
      else if (auto array = dynamic_cast<Array *>(obj))
      {
        for (const auto &elem : array->elements)
        {
          if (elem)
          {
            MarkObject(elem.get(), marked);
          }
        }
      }
      else if (auto hash = dynamic_cast<Hash *>(obj))
      {
        for (const auto &pair : hash->pairs)
        {
          if (pair.second.key)
          {
            MarkObject(pair.second.key.get(), marked);
          }
          if (pair.second.value)
          {
            MarkObject(pair.second.value.get(), marked);
          }
        }
      }
    }

    void Sweep(const std::unordered_set<Object *> &marked)
    {
      // マークされていないオブジェクトを削除
      auto it = store.begin();
      while (it != store.end())
      {
        if (it->second && marked.find(it->second.get()) == marked.end())
        {
          it = store.erase(it);
        }
        else
        {
          ++it;
        }
      }
    }
  };

} // namespace monkey