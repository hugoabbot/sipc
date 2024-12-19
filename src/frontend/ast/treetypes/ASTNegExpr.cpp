#include "ASTNegExpr.h"
#include "ASTVisitor.h"

#include <iostream>

void ASTNegExpr::accept(ASTVisitor *visitor) {
    if(visitor->visit(this)) {
        getExpr()->accept(visitor);
    }
    visitor->endVisit(this);
} // LCOV_EXCL_LINE

std::ostream &ASTNegExpr::print(std::ostream &out) const {
    out << "-(" << *getExpr() << ")";
    return out;
} // LCOV_EXCL_LINE

std::vector<std::shared_ptr<ASTNode>> ASTNegExpr::getChildren() {
    std::vector<std::shared_ptr<ASTNode>> children;
    children.push_back(EXPR);
    return children;
} // LCOV_EXCL_LINE
