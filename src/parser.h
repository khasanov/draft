#pragma once

#include <vector>

#include "token.h"

namespace raft {

class Parser {
public:
    explicit Parser(const std::vector<Token> &tokens);

private:
    template <typename T>
    T *makeAstNode()
    {
        T *node = new (&pool) T{};
        return node;
    }

    template <typename T, typename... Args>
    T *makeAstNode(Args... args)
    {
        T *node = new (&pool) T{std::forward<Args>(args)...};
        return node;
    }

    object::NodePool pool;

    const std::vector<Token> &tokens;
    std::size_t current = 0;
};

}  // namespace raft
