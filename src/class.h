#pragma once

#include "object.h"

namespace raft::object {

class Class : public Callable {
public:
    explicit Class(std::string name);

    std::size_t arity() override;
    object::Object call(Interpreter *, std::vector<Object>) override;
    std::string name;
};

using ClassPtr = std::shared_ptr<Class>;
}  // namespace raft::object
