#pragma once

#include "object.h"

namespace draft::object {

class Callable {
public:
    virtual ~Callable() = default;

    virtual std::size_t arity() = 0;
    virtual Object call(Interpreter *interpreter, std::vector<Object> arguments) = 0;
};

}  // namespace draft::object
