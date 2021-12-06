#include "ast.h"

namespace raft {

Literal::Literal(object::Object value)
    : value{value}
{
}

Unary::Unary(Token op, Expr *right)
    : op{std::move(op)}
    , right{right}
{
}

Binary::Binary(Expr *left, Token op, Expr *right)
    : left{left}
    , op{std::move(op)}
    , right{right}

{
}

Grouping::Grouping(Expr *expr)
    : expression{expr}
{
}

ExprStmt::ExprStmt(Expr *expr)
    : expression{expr}
{
}

Print::Print(Expr *expr)
    : expression{expr}
{
}

}  // namespace raft
