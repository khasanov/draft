#include "resolver.h"

#include "raft.h"

namespace raft {
Resolver::Resolver(Interpreter *interpreter)
    : interpreter{interpreter}
{
}

object::Object Resolver::visit(Literal *)
{
    // there is no work to do
    return object::Null{};
}

object::Object Resolver::visit(Logical *expr)
{
    resolve(expr->left);
    resolve(expr->right);
    return object::Null{};
}

object::Object Resolver::visit(Unary *expr)
{
    resolve(expr->right);
    return object::Null{};
}

object::Object Resolver::visit(Binary *expr)
{
    resolve(expr->left);
    resolve(expr->right);
    return object::Null{};
}

object::Object Resolver::visit(Call *expr)
{
    resolve(expr->callee);
    for (Expr *arg : expr->arguments) {
        resolve(arg);
    }
    return object::Null{};
}

object::Object Resolver::visit(Grouping *expr)
{
    resolve(expr->expression);
    return object::Null{};
}

object::Object Resolver::visit(Variable *expr)
{
    if (!scopes.empty()) {
        auto &scope = scopes.back();
        if (auto it = scope.find(expr->name.lexeme); it != scope.end()) {
            if (!it->second) {
                Raft::error(expr->name.line, "Can't read local variable in its own initializer");
            }
        }
    }
    resolveLocal(expr, expr->name);
    return object::Null{};
}

object::Object Resolver::visit(Assign *expr)
{
    resolve(expr->value);
    resolveLocal(expr, expr->name);
    return object::Null{};
}

object::Object Resolver::visit(Get *expr)
{
    resolve(expr->object);
    return object::Null{};
}

void Resolver::visit(ExprStmt *stmt)
{
    resolve(stmt->expression);
}

void Resolver::visit(If *stmt)
{
    resolve(stmt->condition);
    resolve(stmt->thenBranch);
    if (stmt->elseBranch) {
        resolve(stmt->elseBranch);
    }
}

void Resolver::visit(Function *stmt)
{
    declare(stmt->name);
    define(stmt->name);
    resolveFunction(stmt);
}

void Resolver::visit(Print *stmt)
{
    resolve(stmt->expression);
}

void Resolver::visit(Return *stmt)
{
    if (stmt->value) {
        resolve(stmt->value);
    }
}

void Resolver::visit(While *stmt)
{
    resolve(stmt->condition);
    resolve(stmt->body);
}

void Resolver::visit(Block *stmt)
{
    beginScope();
    resolve(stmt->statements);
    endScope();
}

void Resolver::visit(Class *stmt)
{
    declare(stmt->name);
    define(stmt->name);
}

void Resolver::visit(VarDecl *stmt)
{
    declare(stmt->name);
    if (stmt->initializer) {
        resolve(stmt->initializer);
    }
    define(stmt->name);
}

void Resolver::resolve(const std::vector<Stmt *> &statements)
{
    for (Stmt *statement : statements) {
        resolve(statement);
    }
}

void Resolver::resolve(Stmt *stmt)
{
    stmt->accept(this);
}

void Resolver::resolve(Expr *expr)
{
    expr->accept(this);
}

void Resolver::beginScope()
{
    Scope scope;
    scopes.emplace_back(std::move(scope));
}

void Resolver::endScope()
{
    scopes.pop_back();
}

void Resolver::declare(Token name)
{
    if (scopes.empty()) {
        return;
    }
    auto &scope = scopes.back();
    if (scope.contains(name.lexeme)) {
        Raft::error(name.line, "Already a variable with this name in this scope");
    }
    scope.emplace(name.lexeme, false);
}

void Resolver::define(Token name)
{
    if (scopes.empty()) {
        return;
    }
    scopes.back()[name.lexeme] = true;
}

void Resolver::resolveLocal(Expr *expr, Token name)
{
    for (int i = scopes.size() - 1; i >= 0; i--) {
        if (scopes.at(i).contains(name.lexeme)) {
            interpreter->resolve(expr, scopes.size() - 1 - i);
            return;
        }
    }
}

void Resolver::resolveFunction(Function *function)
{
    beginScope();
    for (Token param : function->params) {
        declare(param);
        define(param);
    }
    resolve(function->body);
    endScope();
}

}  // namespace raft
