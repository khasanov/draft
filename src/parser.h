#pragma once

#include <vector>

#include "ast.h"
#include "token.h"

namespace draft {

class RuntimeError : public std::runtime_error {
public:
    RuntimeError(const Token &token, const std::string &message);

    Token token;
};

class Parser {
public:
    explicit Parser(const std::vector<Token> &tokens);

    std::vector<Stmt *> parse();

private:
    Expr *expression();
    Expr *assignment();
    Expr *equality();
    Expr *comparison();
    Expr *term();
    Expr *factor();
    Expr *logicOr();
    Expr *logicAnd();
    Expr *unary();
    Expr *call();
    Expr *finishCall(Expr *callee);
    Expr *primary();

    Stmt *declaration();
    Stmt *classDeclaration();
    Stmt *funDeclaration();
    FuncStmt *function(std::string kind);
    Stmt *varDeclaration();
    Stmt *statement();
    Stmt *forStatement();
    Stmt *ifStatement();
    Stmt *printStatement();
    Stmt *returnStatement();
    Stmt *whileStatement();
    std::vector<Stmt *> block();
    Stmt *expressionStatement();

    // Checks if we've run out of tokens to parse
    bool isAtEnd();
    // Returns the current token we have yet to consume
    Token peek();
    // Returns the most recently consumed token
    Token previous();
    // Consumes the current token and returns it
    Token advance();
    // Returns true if the current token is of the given kind
    bool check(Token::Kind kind);
    // Checks to see if the next token is of the expected kind
    Token consume(Token::Kind kind, const std::string &msg);
    // Discard tokens until it thinks it has found a statement boundary
    void synchronize();

    // This checks to see if the curret token has any of the given kinds. If so, it consumes the
    // token and returns true. Otherwise it returns false and leaves the current token alone
    template <typename... TokenKind>
    bool match(TokenKind &&... kinds)
    {
        std::vector<Token::Kind> kindvec = {kinds...};
        for (Token::Kind kind : kindvec) {
            if (check(kind)) {
                advance();
                return true;
            }
        }
        return false;
    }

    template <typename T>
    T *makeAstNode()
    {
        T *node = new (&arena) T{};
        return node;
    }

    template <typename T, typename... Args>
    T *makeAstNode(Args... args)
    {
        T *node = new (&arena) T{std::forward<Args>(args)...};
        return node;
    }

    memory::Arena arena;

    const std::vector<Token> &tokens;
    std::size_t current = 0;
};

}  // namespace draft
