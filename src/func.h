#pragma once

#include "environment.h"

namespace raft {
class ReturnEx : public std::runtime_error {
public:
    explicit ReturnEx(object::Object value);
    object::Object value;
};

namespace object {
class Func : public Callable {
public:
    Func(Function *declaration, EnvironmentPtr closure);
    std::size_t arity() override;
    object::Object call(Interpreter *interpreter, std::vector<Object> arguments) override;

private:
    Function *declaration = nullptr;
    EnvironmentPtr closure;
};

class Class : public Callable {
public:
    explicit Class(std::string name);

    std::size_t arity() override;
    object::Object call(Interpreter *, std::vector<Object>) override;
    std::string name;
};
}  // namespace object
}  // namespace raft
