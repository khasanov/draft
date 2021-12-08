#pragma once

#include <string>
#include <variant>

namespace raft::object {

using Null = std::monostate;
using Boolean = bool;
using String = std::string;
using Number = double;
using Object = std::variant<Null, Boolean, String, Number>;

std::string obj2str(const Object &obj);
bool isTruthy(const Object &obj);
bool isEqual(const Object &a, const Object &b);

}  // namespace raft::object
