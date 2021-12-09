#include "interpreter.h"

#include "parser.h"
#include "raft.h"

namespace raft {

Interpreter::Interpreter()
{
    environment = std::make_shared<Environment>();
}

void Interpreter::interpret(const std::vector<Stmt *> &statements)
{
    try {
        for (Stmt *statement : statements) {
            execute(statement);
        }
    } catch (const RuntimeError &err) {
        Raft::error(err.token.line, err.what());
        std::exit(raft::exit::software);
    }
}

object::Object Interpreter::visit(Literal *expr)
{
    return expr->value;
}

object::Object Interpreter::visit(Unary *expr)
{
    object::Object right = evaluate(expr->right);

    switch (expr->op.type) {
    case Token::Type::Minus:
        checkNumberOperand(expr->op, right);
        return -std::get<object::Number>(right);
    case Token::Type::Bang:
        return !object::isTruthy(right);
    default:
        break;
    }

    // Unreachable
    return object::Null{};
}

object::Object Interpreter::visit(Binary *expr)
{
    object::Object left = evaluate(expr->left);
    object::Object right = evaluate(expr->right);

    switch (expr->op.type) {
    case Token::Type::GreaterThanSign:
        checkNumberOperands(expr->op, left, right);
        return std::get<object::Number>(left) > std::get<object::Number>(right);
    case Token::Type::GreaterEqual:
        checkNumberOperands(expr->op, left, right);
        return std::get<object::Number>(left) >= std::get<object::Number>(right);
    case Token::Type::LessThanSign:
        checkNumberOperands(expr->op, left, right);
        return std::get<object::Number>(left) < std::get<object::Number>(right);
    case Token::Type::LessEqual:
        checkNumberOperands(expr->op, left, right);
        return std::get<object::Number>(left) <= std::get<object::Number>(right);
    case Token::Type::BangEqual:
        return !object::isEqual(left, right);
    case Token::Type::EqualEqual:
        return object::isEqual(left, right);
    case Token::Type::Minus:
        checkNumberOperands(expr->op, left, right);
        return std::get<object::Number>(left) - std::get<object::Number>(right);
    case Token::Type::Plus:
        if (std::holds_alternative<object::Number>(left) and std::holds_alternative<object::Number>(right)) {
            return std::get<object::Number>(left) + std::get<object::Number>(right);
        }
        if (std::holds_alternative<object::String>(left) and std::holds_alternative<object::String>(right)) {
            return std::get<object::String>(left) + std::get<object::String>(right);
        }
        throw RuntimeError{expr->op, "Operands must be two numbers or two strings"};
    case Token::Type::Slash:
        checkNumberOperands(expr->op, left, right);
        return std::get<object::Number>(left) / std::get<object::Number>(right);
    case Token::Type::Star:
        checkNumberOperands(expr->op, left, right);
        return std::get<object::Number>(left) * std::get<object::Number>(right);
    default:
        break;
    }

    // Unreachable
    return object::Null{};
}

object::Object Interpreter::visit(Grouping *expr)
{
    return evaluate(expr->expression);
}

object::Object Interpreter::visit(Variable *expr)
{
    return environment->get(expr->name);
}

object::Object Interpreter::visit(Assign *expr)
{
    object::Object value = evaluate(expr->value);
    environment->assign(expr->name, value);
    return value;
}

void Interpreter::visit(ExprStmt *stmt)
{
    evaluate(stmt->expression);
}

void Interpreter::visit(Print *stmt)
{
    object::Object value = evaluate(stmt->expression);
    Raft::out(object::obj2str(value));
}

void Interpreter::visit(Block *stmt)
{
    EnvironmentPtr env = std::make_shared<Environment>(environment);
    executeBlock(stmt->statements, env);
}

void Interpreter::visit(VarDecl *stmt)
{
    object::Object value = object::Null{};
    if (stmt->initializer) {
        value = evaluate(stmt->initializer);
    }
    environment->define(stmt->name.lexeme, value);
}

object::Object Interpreter::evaluate(Expr *expr)
{
    if (expr) {
        return expr->accept(this);
    }
    return object::Null{};
}

void Interpreter::execute(Stmt *stmt)
{
    if (stmt) {
        stmt->accept(this);
    }
}

void Interpreter::executeBlock(const std::vector<Stmt *> &stmts, EnvironmentPtr env)
{
    EnvironmentPtr previous = this->environment;

    this->environment = env;
    for (auto &stmt : stmts) {
        execute(stmt);
    }
    this->environment = previous;
}

void Interpreter::checkNumberOperand(const Token &op, const object::Object &operand)
{
    if (std::holds_alternative<object::Number>(operand)) {
        return;
    }
    throw RuntimeError{op, "Operand must be a number"};
}

void Interpreter::checkNumberOperands(const Token &op, const object::Object &left, const object::Object &right)
{
    if (std::holds_alternative<object::Number>(left) and std::holds_alternative<object::Number>(right)) {
        return;
    }
    throw RuntimeError{op, "Operands must be a numbers"};
}

}  // namespace raft
