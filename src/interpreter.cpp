#include "interpreter.h"

#include <chrono>

#include "parser.h"
#include "raft.h"

namespace raft {

class ClockFunction : public object::Callable {
public:
    std::size_t arity() override
    {
        return 0;
    }
    object::Object call(Interpreter *, std::vector<object::Object>) override
    {
        namespace cr = std::chrono;
        auto now = cr::system_clock::now();
        auto epoch = now.time_since_epoch();
        auto seconds = cr::duration_cast<cr::seconds>(epoch).count();
        return static_cast<object::Number>(seconds);
    }
};

Interpreter::Interpreter()
{
    globals = std::make_shared<Environment>();
    globals->define("clock", std::make_shared<ClockFunction>());
    environment = globals;
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

object::Object Interpreter::visit(Logical *expr)
{
    object::Object left = evaluate(expr->left);

    if (expr->op.kind == Token::Kind::Or) {
        if (object::isTruthy(left)) {
            return left;
        }
    } else {
        if (!object::isTruthy(left)) {
            return left;
        }
    }
    return evaluate(expr->right);
}

object::Object Interpreter::visit(Unary *expr)
{
    object::Object right = evaluate(expr->right);

    switch (expr->op.kind) {
    case Token::Kind::Minus:
        checkNumberOperand(expr->op, right);
        return -std::get<object::Number>(right);
    case Token::Kind::Bang:
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

    switch (expr->op.kind) {
    case Token::Kind::GreaterThanSign:
        checkNumberOperands(expr->op, left, right);
        return std::get<object::Number>(left) > std::get<object::Number>(right);
    case Token::Kind::GreaterEqual:
        checkNumberOperands(expr->op, left, right);
        return std::get<object::Number>(left) >= std::get<object::Number>(right);
    case Token::Kind::LessThanSign:
        checkNumberOperands(expr->op, left, right);
        return std::get<object::Number>(left) < std::get<object::Number>(right);
    case Token::Kind::LessEqual:
        checkNumberOperands(expr->op, left, right);
        return std::get<object::Number>(left) <= std::get<object::Number>(right);
    case Token::Kind::BangEqual:
        return !object::isEqual(left, right);
    case Token::Kind::EqualEqual:
        return object::isEqual(left, right);
    case Token::Kind::Minus:
        checkNumberOperands(expr->op, left, right);
        return std::get<object::Number>(left) - std::get<object::Number>(right);
    case Token::Kind::Plus:
        if (std::holds_alternative<object::Number>(left) and std::holds_alternative<object::Number>(right)) {
            return std::get<object::Number>(left) + std::get<object::Number>(right);
        }
        if (std::holds_alternative<object::String>(left) and std::holds_alternative<object::String>(right)) {
            return std::get<object::String>(left) + std::get<object::String>(right);
        }
        throw RuntimeError{expr->op, "Operands must be two numbers or two strings"};
    case Token::Kind::Slash:
        checkNumberOperands(expr->op, left, right);
        return std::get<object::Number>(left) / std::get<object::Number>(right);
    case Token::Kind::Star:
        checkNumberOperands(expr->op, left, right);
        return std::get<object::Number>(left) * std::get<object::Number>(right);
    default:
        break;
    }

    // Unreachable
    return object::Null{};
}

object::Object Interpreter::visit(Call *expr)
{
    object::Object callee = evaluate(expr->callee);

    std::vector<object::Object> arguments;
    for (Expr *argument : expr->arguments) {
        arguments.emplace_back(evaluate(argument));
    }

    if (!std::holds_alternative<object::CallPtr>(callee)) {
        throw RuntimeError{expr->paren, "Can only call functions and classes"};
    }
    auto function = std::get<object::CallPtr>(callee);
    if (arguments.size() != function->arity()) {
        throw RuntimeError{
            expr->paren,
            "Expected " + std::to_string(function->arity()) + " arguments but got " + std::to_string(arguments.size())};
    }
    return function->call(this, arguments);
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

void Interpreter::visit(If *stmt)
{
    if (object::isTruthy(evaluate(stmt->condition))) {
        execute(stmt->thenBranch);
    } else if (stmt->elseBranch) {
        execute(stmt->elseBranch);
    }
}

void Interpreter::visit(Function *stmt)
{
    auto function = std::make_shared<object::Func>(stmt);
    environment->define(stmt->name.lexeme, function);
}

void Interpreter::visit(Print *stmt)
{
    object::Object value = evaluate(stmt->expression);
    Raft::out(object::obj2str(value));
}

void Interpreter::visit(Return *stmt)
{
    object::Object value = object::Null{};
    if (stmt->value) {
        value = evaluate(stmt->value);
    }
    throw ReturnEx{value};
}

void Interpreter::visit(While *stmt)
{
    while (object::isTruthy(evaluate(stmt->condition))) {
        execute(stmt->body);
    }
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
    throw RuntimeError{op, "Operands must be numbers"};
}

}  // namespace raft
