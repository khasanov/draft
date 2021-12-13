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

If::If(Expr *condition, Stmt *thenBranch, Stmt *elseBranch)
    : condition{condition}
    , thenBranch{thenBranch}
    , elseBranch{elseBranch}
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

Block::Block(const std::vector<Stmt *> &statements)
    : statements{statements}
{
}

}  // namespace raft
