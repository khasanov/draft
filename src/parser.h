#pragma once

#include <vector>

#include "ast.h"
#include "token.h"

namespace raft {

class Parser {
public:
    explicit Parser(const std::vector<Token> &tokens);

private:
    Expr *expression();
    Expr *equality();
    Expr *comparison();
    Expr *term();
    Expr *factor();
    Expr *unary();
    Expr *primary();

    // Checks if we've run out of tokens to parse
    bool isAtEnd();
    // Returns the current token we have yet to consume
    Token peek();
    // Returns the most recently consumed token
    Token previous();
    // Consumes the current token and returns it
    Token advance();
    // Returns true if the current token is of the given type
    bool check(Token::Type type);
    // Checks to see if the next token is of the exprected type
    Token consume(Token::Type type, const std::string &msg);

    // This checks to see if the curret token has any of the given types. If so, it consumes the
    // token and returns true. Otherwise it returns false and leaves the current token alone
    template <typename... TokenType>
    bool match(TokenType &&... types)
    {
        std::vector<Token::Type> typevec = {types...};
        for (Token::Type type : typevec) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }

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
