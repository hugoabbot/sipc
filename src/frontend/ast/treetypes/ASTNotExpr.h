#pragma once
#include "ASTExpr.h"

/*! \brief Class for the unary not operator 
 */
class ASTNotExpr : public ASTExpr {
    std::shared_ptr<ASTExpr> BASE;

public:
    std::vector<std::shared_ptr<ASTNode>> getChildren() override;
    ASTNotExpr(std::shared_ptr<ASTExpr> BASE): BASE(BASE) {}
    ASTExpr* getBase() const { return BASE.get(); }
    void accept(ASTVisitor *visitor) override;
    llvm::Value* codegen() override;

protected:
    std::ostream &print(std::ostream &out) const override;
};
