#pragma once

#include <map>
#include <stack>

#include "ast.h"

namespace raft {
class Interpreter;

class Resolver : public IExprVisitor<object::Object>, IStmtVisitor<void> {
public:
    explicit Resolver(Interpreter *interpreter);

    void resolve(const std::vector<Stmt *> &statements);

private:
    object::Object visit(Literal *expr) override;
    object::Object visit(Logical *expr) override;
    object::Object visit(Unary *expr) override;
    object::Object visit(Binary *expr) override;
    object::Object visit(Call *expr) override;
    object::Object visit(Grouping *expr) override;
    object::Object visit(Variable *expr) override;
    object::Object visit(Assign *expr) override;

    void visit(ExprStmt *stmt) override;
    void visit(If *stmt) override;
    void visit(Function *stmt) override;
    void visit(Print *stmt) override;
    void visit(Return *stmt) override;
    void visit(While *stmt) override;
    void visit(Block *stmt) override;
    void visit(Class *stmt) override;
    void visit(VarDecl *stmt) override;

    void resolve(Stmt *stmt);
    void resolve(Expr *expr);

    void beginScope();
    void endScope();
    void declare(Token name);
    void define(Token name);
    void resolveLocal(Expr *expr, Token name);
    void resolveFunction(Function *function);

    Interpreter *interpreter = nullptr;

    using Scope = std::map<std::string, bool>;
    std::vector<Scope> scopes;
};

}  // namespace raft
