#pragma once

#include <functional>
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace raft {
class Interpreter;
class Function;

namespace object {
class Callable;
class Instance;

using Null = std::monostate;
using Boolean = bool;
using String = std::string;
using Number = double;
using CallPtr = std::shared_ptr<Callable>;
using InstancePtr = std::shared_ptr<Instance>;
using Object = std::variant<Null, Boolean, String, Number, CallPtr, InstancePtr>;

std::string obj2str(const Object &obj);
bool isTruthy(const Object &obj);
bool isEqual(const Object &a, const Object &b);

class Callable {
public:
    virtual ~Callable() = default;

    virtual std::size_t arity() = 0;
    virtual Object call(Interpreter *interpreter, std::vector<Object> arguments) = 0;
};

}  // namespace object
}  // namespace raft
