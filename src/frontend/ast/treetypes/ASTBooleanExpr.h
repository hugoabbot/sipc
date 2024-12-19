#pragma once

#include "ASTExpr.h"

/*! \brief Class for boolean types.
 */
class ASTBooleanExpr : public ASTExpr {
    bool VALUE;

public:
    ASTBooleanExpr(bool VALUE) : VALUE(VALUE) {}
    bool getValue() const { return VALUE; }
    void accept(ASTVisitor *visitor) override;
    llvm::Value* codegen() override;

protected:
    std::ostream &print(std::ostream &out) const override;
};
