#include "ASTArrayDefaultExpr.h"
#include "ASTVisitor.h"
#include "ASTinternal.h"

std::vector<std::shared_ptr<ASTNode>> ASTArrayDefaultExpr::getChildren() {
  std::vector<std::shared_ptr<ASTNode>> children;
  for (const auto &field : FIELDS)
    children.push_back(field);
  return children;
} // LCOV_EXCL_LINE

std::vector<ASTExpr*> ASTArrayDefaultExpr::getFields() const {
  return rawRefs(FIELDS);
} // LCOV_EXCL_LINE

void ASTArrayDefaultExpr::accept(ASTVisitor *visitor) {
  if (visitor->visit(this)) {
    for (ASTExpr *field : getFields())
      field->accept(visitor);
  }
  visitor->endVisit(this);
} // LCOV_EXCL_LINE

std::ostream &ASTArrayDefaultExpr::print(std::ostream &out) const {
  out << "[";
  auto fields = getFields();
  int index = 0;
  for (int i = 0; i < fields.size(); ++i) {
    if (index == fields.size() - 1) {
      out << *fields[i];
    }
    else {
      out << *fields[i] << ", ";
      index++;
    }
  }
  out << "]";
  return out;
} // LCOV_EXCL_LINE
