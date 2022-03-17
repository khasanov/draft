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

using Null = std::monostate;
using Boolean = bool;
using String = std::string;
using Number = double;
using CallPtr = std::shared_ptr<Callable>;
using Object = std::variant<Null, Boolean, String, Number, CallPtr>;

std::string obj2str(const Object &obj);
bool isTruthy(const Object &obj);
bool isEqual(const Object &a, const Object &b);

class Callable {
public:
    virtual ~Callable() = default;

    virtual std::size_t arity() = 0;
    virtual Object call(Interpreter *interpreter, std::vector<Object> arguments) = 0;
};

class Func : public Callable {
public:
    explicit Func(Function *declaration);
    std::size_t arity() override;
    object::Object call(Interpreter *interpreter, std::vector<Object> arguments) override;

private:
    Function *declaration = nullptr;
};

}  // namespace object

class ReturnEx : public std::runtime_error {
public:
    explicit ReturnEx(object::Object value);
    object::Object value;
};

}  // namespace raft
