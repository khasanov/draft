#include "environment.h"

#include "parser.h"

namespace draft {

Environment::Environment(EnvironmentPtr enclosing)
    : enclosing{enclosing}
{
}

void Environment::define(const std::string &name, const object::Object &value)
{
    values[name] = value;
}

object::Object Environment::get(Token name)
{
    if (values.contains(name.lexeme)) {
        return values.at(name.lexeme);
    }
    if (enclosing) {
        return enclosing->get(name);
    }
    throw RuntimeError{name, "Undefined variable '" + name.lexeme + ";"};
}

object::Object Environment::getAt(int distance, std::string name)
{
    return ancestor(distance)->values.at(name);
}

EnvironmentPtr Environment::ancestor(int distance)
{
    EnvironmentPtr env = shared_from_this();
    for (int i = 0; i < distance; i++) {
        env = env->enclosing;
    }
    return env;
}

void Environment::assign(const Token &name, const object::Object &value)
{
    if (values.contains(name.lexeme)) {
        values[name.lexeme] = value;
        return;
    }
    if (enclosing) {
        enclosing->assign(name, value);
        return;
    }
    throw RuntimeError{name, "Undefined variable '" + name.lexeme + "'"};
}

void Environment::assignAt(int distance, const Token &name, const object::Object &value)
{
    ancestor(distance)->values[name.lexeme] = value;
}

}  // namespace draft
