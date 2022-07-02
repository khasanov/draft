#pragma once

#include "environment.h"
#include "obj_callable.h"

namespace draft {
class FuncStmt;

class ReturnEx : public std::runtime_error {
public:
    explicit ReturnEx(object::Object value);
    object::Object value;
};

namespace object {
class Function : public Callable {
public:
    Function(FuncStmt *declaration, EnvironmentPtr closure, bool isInitializer = false);
    std::size_t arity() override;
    object::Object call(Interpreter *interpreter, std::vector<Object> arguments) override;

    std::shared_ptr<Function> bind(std::shared_ptr<Instance> instance);

private:
    FuncStmt *declaration = nullptr;
    EnvironmentPtr closure;
    bool isInitializer = false;
};

using FunctionPtr = std::shared_ptr<Function>;

}  // namespace object
}  // namespace draft
