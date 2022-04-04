#include "ast.h"

namespace raft {

Literal::Literal(object::Object value)
    : value{value}
{
}

Logical::Logical(Expr *left, Token op, Expr *right)
    : left{left}
    , op{op}
    , right{right}
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

While::While(Expr *condition, Stmt *body)
    : condition{condition}
    , body{body}
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

Class::Class(Token name, std::vector<FuncStmt *> methods)
    : name{name}
    , methods{methods}
{
}

Call::Call(Expr *callee, Token paren, std::vector<Expr *> arguments)
    : callee{callee}
    , paren{paren}
    , arguments{arguments}
{
}

FuncStmt::FuncStmt(Token name, std::vector<Token> params, std::vector<Stmt *> body)
    : name{name}
    , params{params}
    , body{body}
{
}

Return::Return(Token keyword, Expr *value)
    : keyword{keyword}
    , value{value}
{
}

Get::Get(Expr *object, Token name)
    : object{object}
    , name{name}
{
}

Set::Set(Expr *object, Token name, Expr *value)
    : object{object}
    , name{name}
    , value{value}
{
}

This::This(Token keyword)
    : keyword{keyword}
{
}

}  // namespace raft
