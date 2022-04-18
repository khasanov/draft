#include "interpreter.h"

#include "builtin.h"
#include "obj_class.h"
#include "obj_instance.h"
#include "object.h"
#include "parser.h"
#include "raft.h"

namespace raft {

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
    case Token::Kind::HyphenMinus:
        checkNumberOperand(expr->op, right);
        return -std::get<object::Number>(right);
    case Token::Kind::ExclamationMark:
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
    case Token::Kind::ExclaimEqual:
        return !object::isEqual(left, right);
    case Token::Kind::EqualEqual:
        return object::isEqual(left, right);
    case Token::Kind::HyphenMinus:
        checkNumberOperands(expr->op, left, right);
        return std::get<object::Number>(left) - std::get<object::Number>(right);
    case Token::Kind::PlusSign:
        if (std::holds_alternative<object::Number>(left) and std::holds_alternative<object::Number>(right)) {
            return std::get<object::Number>(left) + std::get<object::Number>(right);
        }
        if (std::holds_alternative<object::String>(left) and std::holds_alternative<object::String>(right)) {
            return std::get<object::String>(left) + std::get<object::String>(right);
        }
        throw RuntimeError{expr->op, "Operands must be two numbers or two strings"};
    case Token::Kind::Solidus:
        checkNumberOperands(expr->op, left, right);
        return std::get<object::Number>(left) / std::get<object::Number>(right);
    case Token::Kind::Asterisk:
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

    if (!std::holds_alternative<object::CallablePtr>(callee)) {
        throw RuntimeError{expr->paren, "Can only call functions and classes"};
    }
    auto function = std::get<object::CallablePtr>(callee);
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
    return lookUpVariable(expr->name, expr);
}

object::Object Interpreter::visit(Assign *expr)
{
    object::Object value = evaluate(expr->value);
    if (auto it = locals.find(expr); it != locals.end()) {
        int distance = locals.at(expr);
        environment->assignAt(distance, expr->name, value);
    } else {
        globals->assign(expr->name, value);
    }
    return value;
}

object::Object Interpreter::visit(Get *expr)
{
    auto obj = evaluate(expr->object);
    if (std::holds_alternative<object::InstancePtr>(obj)) {
        auto instance = std::get<object::InstancePtr>(obj);
        return instance->getProperty(expr->name.lexeme);
    }
    throw RuntimeError{expr->name, "Only instances have properties"};
}

object::Object Interpreter::visit(Set *expr)
{
    auto obj = evaluate(expr->object);

    if (!std::holds_alternative<object::InstancePtr>(obj)) {
        throw RuntimeError{expr->name, "Only instances have fields"};
    }

    auto value = evaluate(expr->value);
    auto instance = std::get<object::InstancePtr>(obj);
    instance->setProperty(expr->name.lexeme, value);
    return value;
}

object::Object Interpreter::visit(Super *expr)
{
    int distance = locals.at(expr);
    auto superclassObj = environment->getAt(distance, "super");
    object::Callable *superclassCallable = std::get<object::CallablePtr>(superclassObj).get();
    object::Class *superclass = dynamic_cast<object::Class *>(superclassCallable);
    auto instanceObj = environment->getAt(distance - 1, "this");
    object::InstancePtr instance = std::get<object::InstancePtr>(instanceObj);
    auto method = superclass->findMethod(expr->method.lexeme);
    if (!method) {
        Raft::error(expr->method.line, "Undefined property '" + expr->method.lexeme + "'");
    }
    return method->bind(instance);
}

object::Object Interpreter::visit(This *expr)
{
    return lookUpVariable(expr->keyword, expr);
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

void Interpreter::visit(FuncStmt *stmt)
{
    auto function = std::make_shared<object::Function>(stmt, environment, false);
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

void Interpreter::visit(Class *stmt)
{
    object::ClassPtr superclass;
    if (stmt->superclass) {
        auto super = evaluate(stmt->superclass);
        if (!std::holds_alternative<object::CallablePtr>(super) and
            std::dynamic_pointer_cast<object::Class>(std::get<object::CallablePtr>(super))) {  // Ugh!
            Raft::error(stmt->superclass->name.line, "Superclass must be a class");
        } else {
            superclass = std::static_pointer_cast<object::Class>(std::get<object::CallablePtr>(super));
        }
    }
    environment->define(stmt->name.lexeme, object::Null{});

    if (stmt->superclass) {
        environment = std::make_shared<Environment>(environment);
        environment->define("super", superclass);
    }

    std::map<std::string, object::FunctionPtr> methods;
    for (FuncStmt *method : stmt->methods) {
        bool isInitializer = method->name.lexeme == "init";
        auto func = std::make_shared<object::Function>(method, environment, isInitializer);
        std::string methodName = method->name.lexeme;
        auto p = std::make_pair<std::string, object::FunctionPtr>(std::move(methodName), std::move(func));
        methods.emplace(std::move(p));
    }
    auto classObject = std::make_shared<object::Class>(stmt->name.lexeme, superclass, methods);

    if (superclass) {
        environment = environment->enclosing;
    }
    environment->assign(stmt->name, classObject);
}

void Interpreter::visit(Var *stmt)
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

void Interpreter::resolve(Expr *expr, int depth)
{
    locals[expr] = depth;
}

object::Object Interpreter::lookUpVariable(Token name, Expr *expr)
{
    if (auto it = locals.find(expr); it != locals.end()) {
        int distance = locals.at(expr);
        return environment->getAt(distance, name.lexeme);
    } else {
        return globals->get(name);
    }
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
