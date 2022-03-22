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

}  // namespace object
}  // namespace raft
