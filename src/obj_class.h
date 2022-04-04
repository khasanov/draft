#pragma once

#include <map>

#include "obj_callable.h"
#include "obj_function.h"

namespace raft::object {

class Class : public Callable {
public:
    Class(std::string name, std::map<std::string, object::FunctionPtr> methods);

    std::size_t arity() override;
    object::Object call(Interpreter *, std::vector<Object>) override;

    object::FunctionPtr findMethod(std::string name);

    std::string name;

    std::map<std::string, object::FunctionPtr> methods;
};

using ClassPtr = std::shared_ptr<Class>;
}  // namespace raft::object
