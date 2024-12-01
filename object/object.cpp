#include "object.hpp"
#include <sstream>

namespace monkey {

// Integer実装
Integer::Integer(int64_t value) : value_(value) {}

ObjectType Integer::type() const {
    return ObjectType::INTEGER;
}

std::string Integer::inspect() const {
    return std::to_string(value_);
}

int64_t Integer::value() const {
    return value_;
}

// Boolean実装
Boolean::Boolean(bool value) : value_(value) {}

ObjectType Boolean::type() const {
    return ObjectType::BOOLEAN;
}

std::string Boolean::inspect() const {
    return value_ ? "true" : "false";
}

bool Boolean::value() const {
    return value_;
}

// String実装
String::String(const std::string& value) : value_(value) {}

ObjectType String::type() const {
    return ObjectType::STRING;
}

std::string String::inspect() const {
    return value_;
}

const std::string& String::value() const {
    return value_;
}

// Null実装
ObjectType Null::type() const {
    return ObjectType::NULL_OBJ;
}

std::string Null::inspect() const {
    return "null";
}

// Error実装
Error::Error(const std::string& message) : message_(message) {}

ObjectType Error::type() const {
    return ObjectType::ERROR;
}

std::string Error::inspect() const {
    return "ERROR: " + message_;
}

const std::string& Error::message() const {
    return message_;
}

std::string Function::inspect() const {
    std::string params;
    for (size_t i = 0; i < parameters.size(); i++) {
        params += parameters[i];
        if (i < parameters.size() - 1) {
            params += ", ";
        }
    }
    return "fn(" + params + ") {\n" + (body ? body->String() : "") + "\n}";
}

std::string Array::inspect() const {
    std::string result = "[";
    for (size_t i = 0; i < elements.size(); i++) {
        if (elements[i]) {
            result += elements[i]->inspect();
        }
        if (i < elements.size() - 1) {
            result += ", ";
        }
    }
    result += "]";
    return result;
}

std::string Hash::inspect() const {
    std::string result = "{";
    bool first = true;
    for (const auto& pair : pairs) {
        if (!first) {
            result += ", ";
        }
        result += pair.second.key->inspect() + ": " + pair.second.value->inspect();
        first = false;
    }
    result += "}";
    return result;
}

} // namespace monkey