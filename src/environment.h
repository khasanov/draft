#pragma once

#include "object.h"

#include <map>
#include <memory>

#include "token.h"

namespace draft {

class Environment;
using EnvironmentPtr = std::shared_ptr<Environment>;

class Environment : public std::enable_shared_from_this<Environment> {
public:
    Environment() = default;
    explicit Environment(EnvironmentPtr enclosing);
    void define(const std::string &name, const object::Object &value);

    object::Object get(Token name);
    object::Object getAt(int distance, std::string name);
    EnvironmentPtr ancestor(int distance);
    void assign(const Token &name, const object::Object &value);
    void assignAt(int distance, const Token &name, const object::Object &value);

    EnvironmentPtr enclosing = nullptr;
private:
    std::map<std::string, object::Object> values;
};

}  // namespace draft
