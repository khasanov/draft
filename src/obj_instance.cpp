#include "obj_instance.h"

namespace raft::object {

Instance::Instance(Class klass)
    : klass{std::move(klass)}
{
}

Object Instance::getProperty(std::string name)
{
    if (fields.contains(name)) {
        return fields.at(name);
    }

    auto method = klass.findMethod(name);
    if (method) {
        return method.value();
    }
    return Null{};
}

void Instance::setProperty(std::string name, const Object &value)
{
    fields[name] = value;
}

}  // namespace raft::object
