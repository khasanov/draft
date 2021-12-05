#pragma once

#include "ast.h"

namespace raft {

class AstPrinter : public IVisitor<std::string> {
public:
    std::string print(Expr *expr);

    std::string visit(Literal *expr) override;
    std::string visit(Unary *expr) override;
    std::string visit(Binary *expr) override;
    std::string visit(Grouping *expr) override;
};

}  // namespace raft
