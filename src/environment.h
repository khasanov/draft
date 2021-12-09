#pragma once

#include "object.h"

#include <map>
#include <memory>

#include "token.h"

namespace raft {

class Environment;
using EnvironmentPtr = std::shared_ptr<Environment>;

class Environment {
public:
    Environment() = default;
    explicit Environment(EnvironmentPtr enclosing);
    void define(const std::string &name, const object::Object &value);

    object::Object get(Token name);
    void assign(const Token &name, const object::Object &value);

private:
    std::map<std::string, object::Object> values;
    EnvironmentPtr enclosing = nullptr;
};

}  // namespace raft
