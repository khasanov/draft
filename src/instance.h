#pragma once

#include "class.h"

namespace raft::object {

class Instance {
public:
    explicit Instance(Class klass);

private:
    Class klass;
};

}  // namespace raft::object
