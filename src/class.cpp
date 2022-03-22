#include "class.h"

#include "instance.h"

namespace raft::object {
Class::Class(std::string name)
    : name{name}
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

}  // namespace raft::object
