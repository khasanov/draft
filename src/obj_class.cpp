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
    return 0;
}

Object Class::call(Interpreter *, std::vector<Object>)
{
    return std::make_shared<Instance>(*this);
}

std::optional<FunctionPtr> Class::findMethod(std::string name)
{
    if (methods.contains(name)) {
        return methods.at(name);
    }
    return std::nullopt;
}

}  // namespace raft::object
