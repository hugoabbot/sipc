#pragma once

#include "ASTExpr.h"

/*! \brief Class for negated expressions.
 */
class ASTNegExpr : public ASTExpr {
    std::shared_ptr<ASTExpr> EXPR;

public:
    std::vector<std::shared_ptr<ASTNode>> getChildren() override;
    ASTNegExpr(std::shared_ptr<ASTExpr> EXPR) : EXPR(EXPR) {}
    ASTExpr *getExpr() const { return EXPR.get(); }
    void accept(ASTVisitor *visitor) override;
    llvm::Value *codegen() override;

protected:
    std::ostream &print(std::ostream &out) const override;
};
