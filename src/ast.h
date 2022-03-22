#pragma once

#include "arena.h"
#include "object.h"
#include "token.h"

namespace raft {

class Expr;
class Literal;
class Logical;
class Unary;
class Binary;
class Call;
class Grouping;
class Variable;
class Assign;

class Stmt;
class ExprStmt;
class Function;
class If;
class Print;
class Return;
class While;
class Block;
class Class;
class VarDecl;

template <typename T>
class IExprVisitor {
public:
    virtual ~IExprVisitor() = default;
    virtual T visit(Literal *) = 0;
    virtual T visit(Logical *) = 0;
    virtual T visit(Unary *) = 0;
    virtual T visit(Binary *) = 0;
    virtual T visit(Call *) = 0;
    virtual T visit(Grouping *) = 0;
    virtual T visit(Variable *) = 0;
    virtual T visit(Assign *) = 0;
};

class Expr : public memory::Object {
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

class Logical : public ExprNode<Logical> {
public:
    Logical(Expr *left, Token op, Expr *right);

    Expr *left = nullptr;
    Token op;
    Expr *right = nullptr;
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

class Call : public ExprNode<Call> {
public:
    Call(Expr *callee, Token paren, std::vector<Expr *> arguments);
    Expr *callee = nullptr;
    Token paren;
    std::vector<Expr *> arguments;
};

class Grouping : public ExprNode<Grouping> {
public:
    explicit Grouping(Expr *expr);

    Expr *expression = nullptr;
};

class Variable : public ExprNode<Variable> {
public:
    explicit Variable(Token name);

    Token name;
};

class Assign : public ExprNode<Assign> {
public:
    Assign(Token name, Expr *value);

    Token name;
    Expr *value = nullptr;
};

template <typename T>
class IStmtVisitor {
public:
    virtual ~IStmtVisitor() = default;
    virtual T visit(ExprStmt *) = 0;
    virtual T visit(If *) = 0;
    virtual T visit(Function *) = 0;
    virtual T visit(Print *) = 0;
    virtual T visit(Return *) = 0;
    virtual T visit(While *) = 0;
    virtual T visit(Block *) = 0;
    virtual T visit(Class *) = 0;
    virtual T visit(VarDecl *) = 0;
};

class Stmt : public memory::Object {
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

class If : public StmtNode<If> {
public:
    If(Expr *condition, Stmt *thenBranch, Stmt *elseBranch);

    Expr *condition = nullptr;
    Stmt *thenBranch = nullptr;
    Stmt *elseBranch = nullptr;
};

class Function : public StmtNode<Function> {
public:
    Function(Token name, std::vector<Token> params, std::vector<Stmt *> body);

    Token name;
    std::vector<Token> params;
    std::vector<Stmt *> body;
};

class Print : public StmtNode<Print> {
public:
    explicit Print(Expr *expr);

    Expr *expression = nullptr;
};

class Return : public StmtNode<Return> {
public:
    Return(Token keyword, Expr *value);
    Token keyword;
    Expr *value = nullptr;
};

class While : public StmtNode<While> {
public:
    While(Expr *condition, Stmt *body);

    Expr *condition = nullptr;
    Stmt *body = nullptr;
};

class Block : public StmtNode<Block> {
public:
    explicit Block(const std::vector<Stmt *> &statements);

    std::vector<Stmt *> statements;
};

class Class : public StmtNode<Class> {
public:
    Class(Token name, std::vector<Function *> methods);

    Token name;
    std::vector<Function *> methods;
};

class VarDecl : public StmtNode<VarDecl> {
public:
    VarDecl(Token name, Expr *initializer);

    Token name;
    Expr *initializer = nullptr;
};

}  // namespace raft
