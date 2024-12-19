#pragma once

#include "ASTExpr.h"
#include "ASTStmt.h"

/*! \brief Class for an iterator-style for loop.
 */
class ASTForIteratorStmt : public ASTStmt {
    std::shared_ptr<ASTExpr> ELEMENT;
    std::shared_ptr<ASTExpr> ITERABLE;
    std::shared_ptr<ASTStmt> BODY;

public:
    std::vector<std::shared_ptr<ASTNode>> getChildren() override;
    ASTForIteratorStmt(std::shared_ptr<ASTExpr> ELEMENT, std::shared_ptr<ASTExpr> ITERABLE, std::shared_ptr<ASTStmt> BODY)
        : ELEMENT(ELEMENT), ITERABLE(ITERABLE), BODY(BODY) {}
    ASTExpr *getElement() const { return ELEMENT.get(); }
    ASTExpr *getIterable() const { return ITERABLE.get(); }
    ASTStmt *getBody() const { return BODY.get(); }
    void accept(ASTVisitor *visitor) override;
    llvm::Value *codegen() override;

protected:
    std::ostream &print(std::ostream &out) const override;
};