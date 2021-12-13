#pragma once

#include "ast.h"

namespace raft {

class AstPrinter : public IExprVisitor<std::string>, IStmtVisitor<std::string> {
public:
    std::string print(Expr *expr);
    std::string print(Stmt *stmt);

    std::string visit(Literal *expr) override;
    std::string visit(Logical *expr) override;
    std::string visit(Unary *expr) override;
    std::string visit(Binary *expr) override;
    std::string visit(Grouping *expr) override;
    std::string visit(Variable *expr) override;
    std::string visit(Assign *expr) override;

    std::string visit(ExprStmt *stmt) override;
    std::string visit(If *stmt) override;
    std::string visit(Print *stmt) override;
    std::string visit(Block *stmt) override;
    std::string visit(VarDecl *stmt) override;
};

}  // namespace raft
