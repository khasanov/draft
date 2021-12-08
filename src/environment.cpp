#include "environment.h"

#include "parser.h"

namespace raft {

void Environment::define(const std::string &name, const object::Object &value)
{
    values[name] = value;
}

object::Object Environment::get(Token name)
{
    if (values.count(name.lexeme) != 0) {
        return values.at(name.lexeme);
    }

    throw RuntimeError{name, "Undefined variable '" + name.lexeme + ";"};
}

}  // namespace raft
