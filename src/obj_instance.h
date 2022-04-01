#pragma once

#include <map>

#include "obj_class.h"

namespace raft::object {

class Instance {
public:
    explicit Instance(Class klass);

    Object getProperty(std::string name);
    void setProperty(std::string name, const object::Object &value);

private:
    Class klass;
    std::map<std::string, Object> fields;
};

}  // namespace raft::object
