#pragma once

#include <map>

#include "class.h"

namespace raft::object {

class Instance {
public:
    explicit Instance(Class klass);

    Object getProperty(std::string name);

private:
    Class klass;
    std::map<std::string, Object> fields;
};

}  // namespace raft::object
