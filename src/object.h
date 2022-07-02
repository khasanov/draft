#pragma once

#include <functional>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace draft {
class Interpreter;
class Function;

namespace object {
class Callable;
class Instance;

using Null = std::monostate;
using Boolean = bool;
using String = std::string;
using Number = double;
using CallablePtr = std::shared_ptr<Callable>;
using InstancePtr = std::shared_ptr<Instance>;
using Object = std::variant<Null, Boolean, String, Number, CallablePtr, InstancePtr>;

std::string obj2str(const Object &obj);
bool isTruthy(const Object &obj);
bool isEqual(const Object &a, const Object &b);

}  // namespace object
}  // namespace draft
