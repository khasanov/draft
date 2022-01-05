#include "ast_printer.h"

namespace raft {

std::string AstPrinter::print(Expr *expr)
{
    if (expr) {
        return expr->accept(this);
    }
    return "???";
}

std::string AstPrinter::print(Stmt *stmt)
{
    if (stmt) {
        return stmt->accept(this);
    }
    return "???";
}

std::string AstPrinter::visit(Literal *expr)
{
    return "Lit{" + object::obj2str(expr->value) + "}";
}

std::string AstPrinter::visit(Logical *expr)
{
    return "Logic{" + expr->op.lexeme + ", " + expr->left->accept(this) + ", " + expr->right->accept(this) + "}";
}

std::string AstPrinter::visit(Unary *expr)
{
    return "UnOp{'" + expr->op.lexeme + "', " + expr->right->accept(this) + "}";
}

std::string AstPrinter::visit(Binary *expr)
{
    return "BinOp{'" + expr->op.lexeme + "', " + expr->left->accept(this) + ", " + expr->right->accept(this) + "}";
}

std::string AstPrinter::visit(Grouping *expr)
{
    return "{" + expr->expression->accept(this) + "}";
}

std::string AstPrinter::visit(Variable *expr)
{
    return "Var{" + expr->name.lexeme + "}";
}

std::string AstPrinter::visit(Assign *expr)
{
    return "AssignStmt{" + expr->name.lexeme + ", " + expr->value->accept(this) + "}";
}

std::string AstPrinter::visit(ExprStmt *stmt)
{
    return "ExprStmt{" + stmt->expression->accept(this) + "}";
}

std::string AstPrinter::visit(If *stmt)
{
    std::string cond = stmt->condition->accept(this);
    std::string thenBranch = stmt->thenBranch->accept(this);
    std::string elseBranch = stmt->elseBranch ? (", " + stmt->elseBranch->accept(this)) : "";
    return "If{" + cond + ", " + thenBranch + elseBranch + "}";
}

std::string AstPrinter::visit(Print *stmt)
{
    return "PrintStmt{" + stmt->expression->accept(this) + "}";
}

std::string AstPrinter::visit(While *stmt)
{
    return "While{" + stmt->condition->accept(this) + ", " + stmt->body->accept(this) + "}";
}

std::string AstPrinter::visit(Block *stmt)
{
    std::string str = "{";
    const std::size_t size = stmt->statements.size();
    for (std::size_t i = 0; i < size; ++i) {
        auto s = stmt->statements.at(i);
        str += s->accept(this);
        if (i < size - 1) {
            str += ", ";
        }
    }
    str.append("}");
    return str;
}

std::string AstPrinter::visit(VarDecl *stmt)
{
    std::string initializer;
    if (stmt->initializer) {
        initializer = stmt->initializer->accept(this);
    }
    return "VarDecl{" + stmt->name.lexeme + ", " + initializer + "}";
}

}  // namespace raft
