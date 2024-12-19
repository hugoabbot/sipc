#include "ASTArrayLenExpr.h"
#include "ASTVisitor.h"

#include <iostream>

void ASTArrayLenExpr::accept(ASTVisitor *visitor) {
    if(visitor->visit(this)) {
        getArray()->accept(visitor);
    }
    visitor->endVisit(this);
} // LCOV_EXCL_LINE

std::ostream &ASTArrayLenExpr::print(std::ostream &out) const {
    out << "#" << *getArray();
    return out;
} // LCOV_EXCL_LINE

std::vector<std::shared_ptr<ASTNode>> ASTArrayLenExpr::getChildren() {
    std::vector<std::shared_ptr<ASTNode>> children;
    children.push_back(ARRAY);
    return children;
} // LCOV_EXCL_LINE