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
class Get;
class Set;
class Super;
class This;

class Stmt;
class ExprStmt;
class FuncStmt;
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
    virtual T visit(Get *) = 0;
    virtual T visit(Set *) = 0;
    virtual T visit(Super *) = 0;
    virtual T visit(This *) = 0;
};

class Expr : public memory::Object {
public:
    virtual std::string accept(IExprVisitor<std::string> *visitor) = 0;
    virtual object::Object accept(IExprVisitor<object::Object> *visitor) = 0;
};

template <typename T>
class ExprBase : public Expr {
public:
    std::string accept(IExprVisitor<std::string> *visitor) override
    {
        return visitor->visit(static_cast<T *>(this));
    }
    object::Object accept(IExprVisitor<object::Object> *visitor) override
    {
        return visitor->visit(static_cast<T *>(this));
    }
};

class Literal : public ExprBase<Literal> {
public:
    explicit Literal(object::Object value);

    object::Object value;
};

class Logical : public ExprBase<Logical> {
public:
    Logical(Expr *left, Token op, Expr *right);

    Expr *left = nullptr;
    Token op;
    Expr *right = nullptr;
};

class Unary : public ExprBase<Unary> {
public:
    Unary(Token op, Expr *right);

    Token op;
    Expr *right = nullptr;
};

class Binary : public ExprBase<Binary> {
public:
    Binary(Expr *left, Token op, Expr *right);

    Expr *left = nullptr;
    Token op;
    Expr *right = nullptr;
};

class Call : public ExprBase<Call> {
public:
    Call(Expr *callee, Token paren, std::vector<Expr *> arguments);
    Expr *callee = nullptr;
    Token paren;
    std::vector<Expr *> arguments;
};

class Grouping : public ExprBase<Grouping> {
public:
    explicit Grouping(Expr *expr);

    Expr *expression = nullptr;
};

class Variable : public ExprBase<Variable> {
public:
    explicit Variable(Token name);

    Token name;
};

class Assign : public ExprBase<Assign> {
public:
    Assign(Token name, Expr *value);

    Token name;
    Expr *value = nullptr;
};

class Get : public ExprBase<Get> {
public:
    Get(Expr *object, Token name);

    Expr *object = nullptr;
    Token name;
};

class Set : public ExprBase<Set> {
public:
    Set(Expr *object, Token name, Expr *value);

    Expr *object = nullptr;
    Token name;
    Expr *value = nullptr;
};

class Super : public ExprBase<Super> {
public:
    Super(Token keyword, Token method);

    Token keyword;
    Token method;
};
class This : public ExprBase<This> {
public:
    explicit This(Token keyword);
    Token keyword;
};

template <typename T>
class IStmtVisitor {
public:
    virtual ~IStmtVisitor() = default;
    virtual T visit(ExprStmt *) = 0;
    virtual T visit(If *) = 0;
    virtual T visit(FuncStmt *) = 0;
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
class StmtBase : public Stmt {
public:
    std::string accept(IStmtVisitor<std::string> *visitor) override
    {
        return visitor->visit(static_cast<T *>(this));
    }
    void accept(IStmtVisitor<void> *visitor) override
    {
        return visitor->visit(static_cast<T *>(this));
    }
};

class ExprStmt : public StmtBase<ExprStmt> {
public:
    explicit ExprStmt(Expr *expr);

    Expr *expression = nullptr;
};

class If : public StmtBase<If> {
public:
    If(Expr *condition, Stmt *thenBranch, Stmt *elseBranch);

    Expr *condition = nullptr;
    Stmt *thenBranch = nullptr;
    Stmt *elseBranch = nullptr;
};

class FuncStmt : public StmtBase<FuncStmt> {
public:
    FuncStmt(Token name, std::vector<Token> params, std::vector<Stmt *> body);

    Token name;
    std::vector<Token> params;
    std::vector<Stmt *> body;
};

class Print : public StmtBase<Print> {
public:
    explicit Print(Expr *expr);

    Expr *expression = nullptr;
};

class Return : public StmtBase<Return> {
public:
    Return(Token keyword, Expr *value);
    Token keyword;
    Expr *value = nullptr;
};

class While : public StmtBase<While> {
public:
    While(Expr *condition, Stmt *body);

    Expr *condition = nullptr;
    Stmt *body = nullptr;
};

class Block : public StmtBase<Block> {
public:
    explicit Block(const std::vector<Stmt *> &statements);

    std::vector<Stmt *> statements;
};

class Class : public StmtBase<Class> {
public:
    Class(Token name, Variable *superclass, std::vector<FuncStmt *> methods);

    Token name;
    Variable *superclass = nullptr;
    std::vector<FuncStmt *> methods;
};

class VarDecl : public StmtBase<VarDecl> {
public:
    VarDecl(Token name, Expr *initializer);

    Token name;
    Expr *initializer = nullptr;
};

}  // namespace raft
