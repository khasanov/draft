#include "obj_function.h"

#include "interpreter.h"

namespace raft {
ReturnEx::ReturnEx(object::Object value)
    : std::runtime_error{""}
    , value{std::move(value)}
{
}

namespace object {
Function::Function(FuncStmt *declaration, EnvironmentPtr closure)
    : declaration{declaration}
    , closure{closure}
{
}

std::size_t Function::arity()
{
    if (declaration) {
        return declaration->params.size();
    }
    return 0;
}

object::Object Function::call(Interpreter *interpreter, std::vector<object::Object> arguments)
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

std::shared_ptr<Function> Function::bind(std::shared_ptr<Instance> instance)
{
    EnvironmentPtr env = std::make_shared<Environment>(closure);
    env->define("this", std::move(instance));
    return std::make_shared<Function>(declaration, env);
}

}  // namespace object
}  // namespace raft
