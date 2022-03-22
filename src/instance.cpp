#include "instance.h"

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
    return Null{};
}

}  // namespace raft::object
