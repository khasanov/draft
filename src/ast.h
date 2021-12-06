#pragma once

#include "object.h"
#include "token.h"

namespace raft {

class Expr;
class Literal;
class Unary;
class Binary;
class Grouping;

class Stmt;
class ExprStmt;
class Print;

template <typename T>
class IExprVisitor {
public:
    virtual ~IExprVisitor() = default;
    virtual T visit(Literal *) = 0;
    virtual T visit(Unary *) = 0;
    virtual T visit(Binary *) = 0;
    virtual T visit(Grouping *) = 0;
};

class Expr : public object::NodeBase {
public:
    virtual std::string accept(IExprVisitor<std::string> *visitor) = 0;
    virtual object::Object accept(IExprVisitor<object::Object> *visitor) = 0;
};

template <typename T>
class ExprNode : public Expr {
public:
    std::string accept(IExprVisitor<std::string> *visitor) override
    {
        return visitor->visit(static_cast<T *>(this));
    };
    object::Object accept(IExprVisitor<object::Object> *visitor) override
    {
        return visitor->visit(static_cast<T *>(this));
    };
};

class Literal : public ExprNode<Literal> {
public:
    explicit Literal(object::Object value);

    object::Object value;
};

class Unary : public ExprNode<Unary> {
public:
    Unary(Token op, Expr *right);

    Token op;
    Expr *right = nullptr;
};

class Binary : public ExprNode<Binary> {
public:
    Binary(Expr *left, Token op, Expr *right);

    Expr *left = nullptr;
    Token op;
    Expr *right = nullptr;
};

class Grouping : public ExprNode<Grouping> {
public:
    explicit Grouping(Expr *expr);

    Expr *expression = nullptr;
};

template <typename T>
class IStmtVisitor {
public:
    virtual ~IStmtVisitor() = default;
    virtual T visit(ExprStmt *) = 0;
    virtual T visit(Print *) = 0;
};

class Stmt : public object::NodeBase {
public:
    virtual std::string accept(IStmtVisitor<std::string> *visitor) = 0;
    virtual void accept(IStmtVisitor<void> *visitor) = 0;
};

template <typename T>
class StmtNode : public Stmt {
public:
    std::string accept(IStmtVisitor<std::string> *visitor) override
    {
        return visitor->visit(static_cast<T *>(this));
    };
    void accept(IStmtVisitor<void> *visitor) override
    {
        return visitor->visit(static_cast<T *>(this));
    };
};

class ExprStmt : public StmtNode<ExprStmt> {
public:
    explicit ExprStmt(Expr *expr);

    Expr *expression = nullptr;
};

class Print : public StmtNode<Print> {
public:
    Print(Expr *expr);

    Expr *expression = nullptr;
};

}  // namespace raft
