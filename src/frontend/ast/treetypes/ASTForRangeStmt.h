#pragma once

#include "ASTExpr.h"
#include "ASTStmt.h"

/*! \brief Class for a range-style for loop.
 */
class ASTForRangeStmt : public ASTStmt {
    std::shared_ptr<ASTExpr> ELEMENT;
    std::shared_ptr<ASTExpr> LOWER;
    std::shared_ptr<ASTExpr> UPPER;
    std::shared_ptr<ASTExpr> STEP;
    std::shared_ptr<ASTStmt> BODY;

public:
    std::vector<std::shared_ptr<ASTNode>> getChildren() override;
    ASTForRangeStmt(std::shared_ptr<ASTExpr> ELEMENT, std::shared_ptr<ASTExpr> LOWER, std::shared_ptr<ASTExpr> UPPER, std::shared_ptr<ASTExpr> STEP, std::shared_ptr<ASTStmt> BODY)
        : ELEMENT(ELEMENT), LOWER(LOWER), UPPER(UPPER), STEP(STEP), BODY(BODY) {}
    ASTExpr *getElement() const { return ELEMENT.get(); }
    ASTExpr *getLower() const { return LOWER.get(); }
    ASTExpr *getUpper() const { return UPPER.get(); }
    ASTExpr *getStep() const { return STEP.get(); }
    ASTStmt *getBody() const { return BODY.get(); }
    void accept(ASTVisitor *visitor) override;
    llvm::Value *codegen() override;

protected:
    std::ostream &print(std::ostream &out) const override;
};
