#include "obj_class.h"

#include "obj_instance.h"

namespace raft::object {
Class::Class(std::string name, std::map<std::string, object::FunctionPtr> methods)
    : name{name}
    , methods{methods}
{
}

std::size_t Class::arity()
{
    auto initializer = findMethod("init");
    if (initializer) {
        return initializer->arity();
    }
    return 0;
}

Object Class::call(Interpreter *interpreter, std::vector<Object> arguments)
{
    auto instance = std::make_shared<Instance>(*this);
    auto initializer = findMethod("init");
    if (initializer) {
        initializer->bind(instance)->call(interpreter, arguments);
    }
    return instance;
}

FunctionPtr Class::findMethod(std::string name)
{
    if (methods.contains(name)) {
        return methods.at(name);
    }
    return nullptr;
}

}  // namespace raft::object
