#pragma once

#include <string>
#include <variant>

namespace raft::object {

using Null = std::monostate;
using String = std::string;
using Number = double;
using Object = std::variant<Null, String, Number>;

inline std::string obj2str(const Object &obj)
{
    auto visitor = [](const auto &arg) -> std::string {
        using T = std::decay_t<decltype(obj)>;
        std::string ret;
        if constexpr (std::is_same_v<T, String>) {
            ret = arg;
        } else if constexpr (std::is_same_v<T, Number>) {
            ret = std::to_string(arg);
        } else if constexpr (std::is_same_v<T, Null>) {
            ret = "nil";
        }
        return ret;
    };
    return std::visit(visitor, obj);
}
}  // namespace raft::object
