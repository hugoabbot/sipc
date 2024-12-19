#pragma once

#include "ASTExpr.h"

/*! \brief Class for a default array
 */
class ASTArrayDefaultExpr : public ASTExpr {
  std::vector<std::shared_ptr<ASTExpr>> FIELDS;

public: 
  std::vector<std::shared_ptr<ASTNode>> getChildren() override;
  ASTArrayDefaultExpr(std::vector<std::shared_ptr<ASTExpr>> FIELDS) : FIELDS(FIELDS) {}
  std::vector<ASTExpr *> getFields() const;
  void accept(ASTVisitor *visitor) override;
  llvm::Value* codegen() override;

protected:
  std::ostream &print(std::ostream &out) const override;
};
