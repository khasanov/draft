#include "obj_instance.h"

namespace draft::object {

Instance::Instance(Class klass)
    : klass{std::move(klass)}
{
}

Object Instance::getProperty(std::string name)
{
    if (fields.contains(name)) {
        return fields.at(name);
    }

    FunctionPtr method = klass.findMethod(name);
    if (method) {
        return method->bind(shared_from_this());
    }
    return Null{};
}

void Instance::setProperty(std::string name, const Object &value)
{
    fields[name] = value;
}

}  // namespace draft::object
