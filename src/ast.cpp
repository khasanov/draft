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

Variable::Variable(Token name)
    : name{name}
{
}

Assign::Assign(Token name, Expr *value)
    : name{name}
    , value{value}
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

VarDecl::VarDecl(Token name, Expr *initializer)
    : name{name}
    , initializer{initializer}
{
}

}  // namespace raft
