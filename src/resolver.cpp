#include "resolver.h"

#include "driver.h"

namespace draft {
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
                Driver::error(expr->name.line, "Can't read local variable in its own initializer");
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

object::Object Resolver::visit(Set *expr)
{
    resolve(expr->value);
    resolve(expr->object);
    return object::Null{};
}

object::Object Resolver::visit(Super *expr)
{
    if (currentClass == ClassType::None) {
        Driver::error(expr->keyword.line, "Can't use 'super' outside a class");
    } else if (currentClass != ClassType::Subclass) {
        Driver::error(expr->keyword.line, "Can't use 'super' in a class with no superclass");
    }
    resolveLocal(expr, expr->keyword);
    return object::Null{};
}

object::Object Resolver::visit(This *expr)
{
    if (currentClass == ClassType::None) {
        Driver::error(expr->keyword.line, "Can't use 'this' outside of a class");
        return object::Null{};
    }
    resolveLocal(expr, expr->keyword);
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

void Resolver::visit(FuncStmt *stmt)
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
    if (currentFunction == FunctionType::None) {
        Driver::error(stmt->keyword.line, "Can't return from top-level code");
    }
    if (stmt->value) {
        if (currentFunction == FunctionType::Initializer) {
            Driver::error(stmt->keyword.line, "Can't return a value from an initializer");
        }
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
    ClassType enclosingClass = currentClass;
    currentClass = ClassType::Class;

    declare(stmt->name);
    define(stmt->name);
    if (stmt->superclass) {
        if (stmt->name.lexeme == stmt->superclass->name.lexeme) {
            Driver::error(stmt->superclass->name.line, "A class can't inherit from itself");
        } else {
            currentClass = ClassType::Subclass;
            resolve(stmt->superclass);
        }

        beginScope();
        scopes.back()["super"] = true;
    }
    beginScope();
    auto back = scopes.back();
    back["this"] = true;
    scopes.pop_back();
    scopes.emplace_back(back);

    for (FuncStmt *method : stmt->methods) {
        FunctionType declaration = FunctionType::Method;
        if (method->name.lexeme == "init") {
            declaration = FunctionType::Initializer;
        }
        resolveFunction(method, declaration);
    }
    endScope();
    if (stmt->superclass) {
        endScope();
    }
    currentClass = enclosingClass;
}

void Resolver::visit(Var *stmt)
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
        Driver::error(name.line, "Already a variable with this name in this scope");
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

void Resolver::resolveFunction(FuncStmt *function, FunctionType type)
{
    FunctionType enclosing = currentFunction;
    currentFunction = type;
    beginScope();
    for (Token param : function->params) {
        declare(param);
        define(param);
    }
    resolve(function->body);
    endScope();
    currentFunction = enclosing;
}

}  // namespace draft
