#pragma once

#include "obj_callable.h"

namespace raft {

class ClockFunction : public object::Callable {
public:
    std::size_t arity() override;

    object::Object call(Interpreter *, std::vector<object::Object>) override;
};

}  // namespace raft
