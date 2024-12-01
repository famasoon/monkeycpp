#include "object.hpp"
#include <sstream>

namespace monkey
{

  // Integer implementation
  Integer::Integer(int64_t value) : value_(value) {}

  ObjectType Integer::type() const
  {
    return ObjectType::INTEGER;
  }

  std::string Integer::inspect() const
  {
    return std::to_string(value_);
  }

  int64_t Integer::value() const
  {
    return value_;
  }

  // Boolean implementation
  Boolean::Boolean(bool value) : value_(value) {}

  ObjectType Boolean::type() const
  {
    return ObjectType::BOOLEAN;
  }

  std::string Boolean::inspect() const
  {
    return value_ ? "true" : "false";
  }

  bool Boolean::value() const
  {
    return value_;
  }

  // String implementation
  String::String(std::string value) : value_(std::move(value)) {}

  ObjectType String::type() const
  {
    return ObjectType::STRING;
  }

  std::string String::inspect() const
  {
    return value_;
  }

  const std::string &String::getValue() const
  {
    return value_;
  }

  // Null implementation
  ObjectType Null::type() const
  {
    return ObjectType::NULL_OBJ;
  }

  std::string Null::inspect() const
  {
    return "null";
  }

  // Error implementation
  Error::Error(const std::string &message) : message_(message) {}

  ObjectType Error::type() const
  {
    return ObjectType::ERROR;
  }

  std::string Error::inspect() const
  {
    return "ERROR: " + message_;
  }

  const std::string &Error::message() const
  {
    return message_;
  }

  // ReturnValue implementation
  ReturnValue::ReturnValue(ObjectPtr v) : value(std::move(v))
  {
    if (!value)
    {
      value = std::make_shared<Null>();
    }
  }

  ObjectType ReturnValue::type() const
  {
    return ObjectType::RETURN_VALUE;
  }

  std::string ReturnValue::inspect() const
  {
    return value->inspect();
  }

  // Function implementation
  Function::Function(std::vector<std::string> params, const AST::BlockStatement *b, EnvPtr e)
      : parameters(std::move(params)), body(b), env(std::move(e)) {}

  ObjectType Function::type() const
  {
    return ObjectType::FUNCTION;
  }

  std::string Function::inspect() const
  {
    std::string result;
    result.reserve(128);

    result += "fn(";
    if (!parameters.empty())
    {
      result += parameters[0];
      for (size_t i = 1; i < parameters.size(); ++i)
      {
        result += ", ";
        result += parameters[i];
      }
    }
    result += ") {\n";
    if (body)
    {
      result += body->String();
    }
    result += "\n}";
    return result;
  }

  // Builtin implementation
  Builtin::Builtin(BuiltinFunction function) : fn(std::move(function)) {}

  ObjectType Builtin::type() const
  {
    return ObjectType::BUILTIN;
  }

  std::string Builtin::inspect() const
  {
    return "builtin function";
  }

  // Array implementation
  Array::Array(std::vector<ObjectPtr> elems) : elements(std::move(elems)) {}

  ObjectType Array::type() const
  {
    return ObjectType::ARRAY;
  }

  std::string Array::inspect() const
  {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < elements.size(); ++i)
    {
      if (elements[i])
      {
        ss << elements[i]->inspect();
      }
      if (i < elements.size() - 1)
      {
        ss << ", ";
      }
    }
    ss << "]";
    return ss.str();
  }

  // Hash implementation
  ObjectType Hash::type() const
  {
    return ObjectType::HASH;
  }

  std::string Hash::inspect() const
  {
    std::stringstream ss;
    ss << "{";
    bool first = true;
    for (const auto &pair : pairs)
    {
      if (!first)
      {
        ss << ", ";
      }
      if (pair.second.key && pair.second.value)
      {
        ss << pair.second.key->inspect() << ": " << pair.second.value->inspect();
      }
      first = false;
    }
    ss << "}";
    return ss.str();
  }

  // HashPair implementation
  HashPair::HashPair(ObjectPtr k, ObjectPtr v) : key(std::move(k)), value(std::move(v)) {}

  // Environment implementation
  EnvPtr Environment::NewEnvironment()
  {
    return std::make_shared<Environment>();
  }

  EnvPtr Environment::NewEnclosedEnvironment(EnvPtr outer)
  {
    auto env = NewEnvironment();
    env->outer = outer;
    return env;
  }

  ObjectPtr Environment::Get(const std::string &name)
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

    return std::make_shared<Error>("identifier not found: " + name);
  }

  ObjectPtr Environment::Set(const std::string &name, ObjectPtr val)
  {
    store[name] = val;
    return val;
  }

  void Environment::MarkAndSweep()
  {
    std::unordered_set<Object *> marked;
    marked.reserve(store.size());
    Mark(marked);
    Sweep(marked);
  }

  void Environment::Mark(std::unordered_set<Object *> &marked)
  {
    marked.reserve(marked.size() + store.size());
    for (const auto &pair : store)
    {
      if (pair.second)
      {
        MarkObject(pair.second.get(), marked);
      }
    }

    if (auto outerEnv = outer.lock())
    {
      outerEnv->Mark(marked);
    }
  }

  void Environment::MarkObject(Object *obj, std::unordered_set<Object *> &marked)
  {
    if (!obj || marked.find(obj) != marked.end())
    {
      return;
    }

    marked.insert(obj);

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

  void Environment::Sweep(const std::unordered_set<Object *> &marked)
  {
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

  // その他の既存の実装はそのまま維持...

} // namespace monkey