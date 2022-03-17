#include "func.h"

#include "interpreter.h"

namespace raft {
ReturnEx::ReturnEx(object::Object value)
    : std::runtime_error{""}
    , value{value}
{
}

namespace object {
Func::Func(Function *declaration, EnvironmentPtr closure)
    : declaration{declaration}
    , closure{closure}
{
}

std::size_t Func::arity()
{
    if (declaration) {
        return declaration->params.size();
    }
    return 0;
}

object::Object Func::call(Interpreter *interpreter, std::vector<object::Object> arguments)
{
    if (!declaration) {
        return Null{};
    }
    EnvironmentPtr env = std::make_shared<Environment>(closure);
    auto params = declaration->params;
    for (std::size_t i = 0; i < params.size(); ++i) {
        env->define(params.at(i).lexeme, arguments.at(i));
    }
    try {
        interpreter->executeBlock(declaration->body, env);
    } catch (const ReturnEx &returnValue) {
        return returnValue.value;
    }

    return Null{};
}
}  // namespace object
}  // namespace raft
