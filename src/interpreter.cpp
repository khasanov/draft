#include "interpreter.h"

#include "parser.h"
#include "raft.h"

namespace raft {

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

void Interpreter::visit(ExprStmt *stmt)
{
    evaluate(stmt->expression);
}

void Interpreter::visit(Print *stmt)
{
    object::Object value = evaluate(stmt->expression);
    Raft::out(object::obj2str(value));
}

object::Object Interpreter::evaluate(Expr *expr)
{
    return expr->accept(this);
}

void Interpreter::execute(Stmt *stmt)
{
    stmt->accept(this);
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
