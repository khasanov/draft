#pragma once

#include "ast.h"

namespace draft {

class AstPrinter : public IExprVisitor<std::string>, IStmtVisitor<std::string> {
public:
    std::string print(Expr *expr);
    std::string print(Stmt *stmt);

    std::string visit(Literal *expr) override;
    std::string visit(Logical *expr) override;
    std::string visit(Unary *expr) override;
    std::string visit(Binary *expr) override;
    std::string visit(Call *expr) override;
    std::string visit(Grouping *expr) override;
    std::string visit(Variable *expr) override;
    std::string visit(Assign *expr) override;
    std::string visit(Get *expr) override;
    std::string visit(Set *expr) override;
    std::string visit(Super *expr) override;
    std::string visit(This *expr) override;

    std::string visit(ExprStmt *stmt) override;
    std::string visit(If *stmt) override;
    std::string visit(FuncStmt *stmt) override;
    std::string visit(Print *stmt) override;
    std::string visit(Return *stmt) override;
    std::string visit(While *stmt) override;
    std::string visit(Block *stmt) override;
    std::string visit(Class *stmt) override;
    std::string visit(Var *stmt) override;
};

}  // namespace draft
