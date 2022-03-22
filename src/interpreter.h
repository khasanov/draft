#pragma once

#include "ast.h"
#include "environment.h"
#include "func.h"

#include <vector>

namespace raft {

class Interpreter : public IExprVisitor<object::Object>, IStmtVisitor<void> {
public:
    Interpreter();
    void interpret(const std::vector<Stmt *> &statements);

    object::Object visit(Literal *expr) override;
    object::Object visit(Logical *expr) override;
    object::Object visit(Unary *expr) override;
    object::Object visit(Binary *expr) override;
    object::Object visit(Call *expr) override;
    object::Object visit(Grouping *expr) override;
    object::Object visit(Variable *expr) override;
    object::Object visit(Assign *expr) override;
    object::Object visit(Get *expr) override;

    void visit(ExprStmt *stmt) override;
    void visit(If *stmt) override;
    void visit(Function *stmt) override;
    void visit(Print *stmt) override;
    void visit(Return *stmt) override;
    void visit(While *stmt) override;
    void visit(Block *stmt) override;
    void visit(Class *stmt) override;
    void visit(VarDecl *stmt) override;

    void resolve(Expr *expr, int depth);

private:
    object::Object evaluate(Expr *expr);
    void execute(Stmt *stmt);
    void executeBlock(const std::vector<Stmt *> &stmts, EnvironmentPtr env);
    object::Object lookUpVariable(Token name, Expr *expr);

    void checkNumberOperand(const Token &op, const object::Object &operand);
    void checkNumberOperands(const Token &op, const object::Object &left, const object::Object &right);

    std::map<Expr *, int> locals;
    EnvironmentPtr globals;
    EnvironmentPtr environment;

    friend class object::Func;
};

}  // namespace raft
