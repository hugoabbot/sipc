#include "ASTArrayFixedExpr.h"
#include "ASTVisitor.h"

std::vector<std::shared_ptr<ASTNode>> ASTArrayFixedExpr::getChildren() {
  std::vector<std::shared_ptr<ASTNode>> children;
  children.push_back(NUMBER);
  children.push_back(INSTANCE);
  return children;
} // LCOV_EXCL_LINE

void ASTArrayFixedExpr::accept(ASTVisitor *visitor) {
  if (visitor->visit(this)) {
    getNumber()->accept(visitor);
    getInstance()->accept(visitor);
  }
  visitor->endVisit(this);
} // LCOV_EXCL_LINE

std::ostream &ASTArrayFixedExpr::print(std::ostream &out) const {
  out << "[" << *getNumber() << " of " << *getInstance() << "]"; 
  return out;
} // LCOV_EXCL_LINE