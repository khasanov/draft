#pragma once

#include "object.h"

#include <map>

#include "token.h"

namespace raft {

class Environment {
public:
    void define(const std::string &name, const object::Object &value);

    object::Object get(Token name);
    void assign(const Token &name, const object::Object &value);

private:
    std::map<std::string, object::Object> values;
};

}  // namespace raft
