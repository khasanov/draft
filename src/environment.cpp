#include "environment.h"

#include "parser.h"

namespace raft {

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
    if (values.count(name.lexeme) != 0) {
        return values.at(name.lexeme);
    }
    if (enclosing) {
        return enclosing.get();
    }
    throw RuntimeError{name, "Undefined variable '" + name.lexeme + ";"};
}

void Environment::assign(const Token &name, const object::Object &value)
{
    if (values.count(name.lexeme) != 0) {
        values[name.lexeme] = value;
        return;
    }
    if (enclosing) {
        enclosing->assign(name, value);
        return;
    }
    throw RuntimeError{name, "Undefined variable '" + name.lexeme + "'"};
}

}  // namespace raft
