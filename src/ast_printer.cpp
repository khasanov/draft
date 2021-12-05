#include "ast_printer.h"

namespace raft {

std::string AstPrinter::print(Expr *expr)
{
    return expr->accept(this);
}

std::string AstPrinter::visit(Literal *expr)
{
    return "Lit{" + object::obj2str(expr->value) + "}";
}

std::string AstPrinter::visit(Unary *expr)
{
    return "UnOp{\"" + expr->op.lexeme + "\", " + expr->right->accept(this) + "}";
}

std::string AstPrinter::visit(Binary *expr)
{
    return "BinOp{\"" + expr->op.lexeme + "\", " + expr->left->accept(this) + ", " + expr->right->accept(this) + "}";
}

std::string AstPrinter::visit(Grouping *expr)
{
    return "{" + expr->expression->accept(this) + "}";
}

}  // namespace raft
