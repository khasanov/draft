#include "instance.h"

namespace raft::object {

Instance::Instance(Class klass)
    : klass{std::move(klass)}
{
}

}  // namespace raft::object
