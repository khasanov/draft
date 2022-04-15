#pragma once

#include <map>

#include "obj_callable.h"
#include "obj_function.h"

namespace raft::object {
class Class;
using ClassPtr = std::shared_ptr<Class>;

class Class : public Callable {
public:
    Class(std::string name, ClassPtr superclass, std::map<std::string, object::FunctionPtr> methods);

    std::size_t arity() override;
    object::Object call(Interpreter *interpreter, std::vector<Object> arguments) override;

    object::FunctionPtr findMethod(std::string name);

    std::string name;

    std::map<std::string, object::FunctionPtr> methods;
    ClassPtr superclass;
};

}  // namespace raft::object
