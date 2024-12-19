#pragma once

#include "ASTExpr.h"
#include "ASTStmt.h"

/*! \brief Class for decrement statement.
 */
class ASTDecrementStmt : public ASTStmt {
    std::shared_ptr<ASTExpr> BASE;

public:
    std::vector<std::shared_ptr<ASTNode>> getChildren() override;
    ASTDecrementStmt(std::shared_ptr<ASTExpr> BASE) : BASE(BASE) {}
    ASTExpr* getBase() const { return BASE.get(); }
    void accept(ASTVisitor *visitor) override;
    llvm::Value* codegen() override;

protected:
    std::ostream &print(std::ostream &out) const override;
};
