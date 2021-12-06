#pragma once

#include "ast.h"

namespace raft {

class Interpreter : public IVisitor<object::Object> {

public:
    void interpret(Expr *expression);

    object::Object visit(Literal *expr) override;
    object::Object visit(Unary *expr) override;
    object::Object visit(Binary *expr) override;
    object::Object visit(Grouping *expr) override;

private:
    object::Object evaluate(Expr *expr);

    void checkNumberOperand(const Token &op, const object::Object &operand);
    void checkNumberOperands(const Token &op, const object::Object &left, const object::Object &right);
};

}  // namespace raft
