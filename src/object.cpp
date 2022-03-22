#include "object.h"

#include <iostream>
#include <stdexcept>

namespace raft::object {

std::string obj2str(const Object &obj)
{
    auto visitor = [](auto &&arg) -> std::string {
        using T = std::decay_t<decltype(arg)>;
        std::string ret;
        if constexpr (std::is_same_v<T, String>) {
            ret = arg;
        } else if constexpr (std::is_same_v<T, Number>) {
            ret = std::to_string(arg);
        } else if constexpr (std::is_same_v<T, Boolean>) {
            ret = arg ? "true" : "false";
        } else if constexpr (std::is_same_v<T, Null>) {
            ret = "nil";
        } else if constexpr (std::is_same_v<T, CallPtr>) {
            ret = "callable";
        } else if constexpr (std::is_same_v<T, InstancePtr>) {
            ret = "instance";
        } else {
            throw std::runtime_error{"Unknown Object type"};
        }
        return ret;
    };
    return std::visit(visitor, obj);
}

// false and nil are falsey, and everything else is truthy
bool isTruthy(const Object &obj)
{
    if (std::holds_alternative<Null>(obj)) {
        return false;
    }
    if (std::holds_alternative<Boolean>(obj)) {
        return std::get<bool>(obj);
    }
    return true;
}

bool isEqual(const Object &a, const Object &b)
{
    if (std::holds_alternative<Null>(a) and std::holds_alternative<Null>(b)) {
        return true;
    }
    if (std::holds_alternative<Null>(a)) {
        return false;
    }
    return a == b;
}

}  // namespace raft::object
