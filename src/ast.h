#pragma once

#include "object.h"
#include "token.h"

namespace raft {

class Expr;
class Literal;
class Unary;
class Binary;
class Grouping;

template <typename T>
class IVisitor {
public:
    virtual ~IVisitor() = default;
    virtual T visit(Literal *) = 0;
    virtual T visit(Unary *) = 0;
    virtual T visit(Binary *) = 0;
    virtual T visit(Grouping *) = 0;
};

class Expr : public object::NodeBase {
public:
    virtual void accept(IVisitor<void> *visitor) = 0;
    virtual std::string accept(IVisitor<std::string> *visitor) = 0;
};

template <typename T>
class Node : public Expr {
public:
    void accept(IVisitor<void> *visitor) override
    {
        visitor->visit(static_cast<T *>(this));
    }
    std::string accept(IVisitor<std::string> *visitor) override
    {
        return visitor->visit(static_cast<T *>(this));
    };
};

class Literal : public Node<Literal> {
public:
    explicit Literal(object::Object value);

    object::Object value;
};

class Unary : public Node<Unary> {
public:
    Unary(Token op, Expr *right);

    Token op;
    Expr *right;
};

class Binary : public Node<Binary> {
public:
    Binary(Expr *left, Token op, Expr *right);

    Expr *left;
    Token op;
    Expr *right;
};

class Grouping : public Node<Grouping> {
public:
    explicit Grouping(Expr *expr);

    Expr *expression;
};

}  // namespace raft
