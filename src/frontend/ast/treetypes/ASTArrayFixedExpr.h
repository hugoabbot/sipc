#pragma once

#include "ASTExpr.h"

/*! \brief Class for a fixed array 
 */
class ASTArrayFixedExpr : public ASTExpr {
  std::shared_ptr<ASTExpr> NUMBER, INSTANCE;

public: 
  std::vector<std::shared_ptr<ASTNode>> getChildren() override;
  ASTArrayFixedExpr(std::shared_ptr<ASTExpr> NUMBER, std::shared_ptr<ASTExpr> INSTANCE) : NUMBER(NUMBER), INSTANCE(INSTANCE) {}
  ASTExpr *getNumber() const { return NUMBER.get(); }
  ASTExpr *getInstance() const { return INSTANCE.get(); }
  void accept(ASTVisitor *visitor) override;
  llvm::Value* codegen() override;

protected:
  std::ostream &print(std::ostream &out) const override;
};